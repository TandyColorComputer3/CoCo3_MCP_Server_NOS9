import crypto from 'node:crypto';

export async function requalifyAfterMount({ bridge, mount, images, expectedImage, ready,
    decodeConsole, rememberShell, forgetShell, nonce = () => crypto.randomBytes(16).toString('hex'),
    challengeNonce = () => crypto.randomBytes(12).toString('hex'),
    poll = () => new Promise(resolve => setTimeout(resolve, 50)), now = () => Date.now(),
    timeoutMs = 20000 }) {
  forgetShell(bridge);
  const proof = { trusted: false, mountVerified: false, leaseAcquired: false,
    challengeSubmitted: false, challengeVerified: false, leaseReleased: false };
  let token;
  let acquired = false;
  try {
    if (mount?.briefname !== 'flop2' || mount.filename !== expectedImage)
      throw new Error('mount result does not identify expected flop2 image');
    if (!Array.isArray(images) || !images.some(x => x.briefname === 'flop2'
        && x.filename === expectedImage && x.exists === true))
      throw new Error('image-device listing does not identify expected flop2 image');
    proof.mountVerified = true;
    const prompt = ready?.handshake?.prompt;
    const epoch = ready?.postLoadEpoch;
    if (ready?.ready !== true || ready?.loadCompleted !== true || ready?.shellVerified !== true
        || typeof prompt !== 'string' || !/^MCP[0-9a-f]{16}:$/.test(prompt)
        || !Number.isInteger(epoch) || epoch < 0)
      throw new Error('invalid current-session ready result');
    proof.prompt = prompt;
    proof.epoch = epoch;
    token = nonce();
    if (!/^[0-9a-f]{32}$/.test(token)) throw new Error('invalid fresh lease token');
    const deadline = now() + timeoutMs;
    async function request(cmd, params = {}) {
      if (now() >= deadline) throw new Error(`requalification timeout before ${cmd}`);
      const response = await bridge.request(cmd, { ...params, token, epoch }, 2000);
      if (!response?.ok) throw new Error(`${cmd}: ${response?.error ?? 'invalid bridge response'}`);
      if (!response.result || typeof response.result !== 'object') throw new Error(`${cmd}: missing result`);
      return response.result;
    }
    const lease = await request('begin_run');
    acquired = true;
    if (lease.token !== token || lease.epoch !== epoch) throw new Error('invalid console lease response');
    proof.leaseAcquired = true;
    async function screen() {
      const data = await request('read_text_console');
      if (data.epoch !== epoch) throw new Error('console epoch mismatch');
      return { rows: decodeConsole(data), idle: data.idle === true };
    }
    const initial = await screen();
    const last = rows => rows.filter(x => x !== '').at(-1) ?? '';
    if (!initial.idle || last(initial.rows) !== prompt) throw new Error('expected nonce prompt is not idle');
    const challenge = challengeNonce();
    if (!/^[0-9a-f]{24}$/.test(challenge)) throw new Error('invalid fresh challenge nonce');
    const marker = `MCPCHK${challenge}`;
    proof.marker = marker;
    if (initial.rows.some(row => row.includes(marker))) throw new Error('challenge marker already present');
    await request('type', { text: `echo ${marker}{ENTER}` });
    proof.challengeSubmitted = true;
    for (;;) {
      const current = await screen();
      const echoIndex = current.rows.findIndex(row => row === `${prompt}echo ${marker}`);
      const markerIndex = current.rows.findIndex(row => row === marker);
      const promptIndex = current.rows.findLastIndex(row => row === prompt);
      if (current.idle && echoIndex >= 0 && markerIndex > echoIndex && promptIndex > markerIndex
          && promptIndex === current.rows.findLastIndex(row => row !== '')) {
        proof.challengeVerified = true;
        proof.challengeRows = { echoIndex, markerIndex, promptIndex };
        break;
      }
      if (now() >= deadline) throw new Error('fresh challenge marker and prompt not observed in order');
      await poll();
    }
    const status = await request('load_state_status');
    if (status.token !== token || status.epoch !== epoch || status.completed !== true
        || status.invalidated !== false)
      throw new Error('console lease/epoch invalidated after challenge');
    proof.leaseReconfirmed = true;
  } catch (error) {
    proof.failure = error instanceof Error ? error.message : String(error);
  } finally {
    if (acquired) {
      try {
        const end = await bridge.request('finish_run', { token }, 1000);
        if (!end?.ok || end.result?.released !== true || end.result?.invalidated !== false)
          throw new Error(`finish_run rejected or invalidated: ${end?.error ?? JSON.stringify(end?.result)}`);
        proof.leaseReleased = true;
      } catch (error) {
        proof.releaseFailure = error instanceof Error ? error.message : String(error);
      }
    }
  }
  if (!proof.failure && !proof.releaseFailure && proof.leaseReconfirmed && proof.leaseReleased) {
    rememberShell(bridge, ready);
    proof.trusted = true;
  } else {
    forgetShell(bridge);
  }
  return proof;
}
