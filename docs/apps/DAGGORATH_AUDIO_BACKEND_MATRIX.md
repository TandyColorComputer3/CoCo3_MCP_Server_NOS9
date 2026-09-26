# Daggorath audio backends: installed MAME feasibility

Research date: 2026-09-26. Installed build: **MAME 0.289 (ample-289-u2)**,
`/Applications/Emulators/Ample.app/Contents/MacOS/mame64`.

## Decision

**Next automated experiment: Tandy Speech/Sound Pak sound synthesis.** Its
controller firmware, PSG, speech processor and output routing are implemented;
its required ROMs pass the installed MAME audit. It is already in canonical MPI
slot 2. This establishes a credible executable test target, **not a completed
audio test**. This pass sent no sound/MIDI commands and implemented no backend.

GMC is the next useful independent synthesis target. CoCo PSG is also usable for
functional development, with an unresolved absolute-clock discrepancy. MIDI Pak
needs a host endpoint and guest interface; DriveWire MIDI needs a working
DriveWire stack/server as well. CoCo DAC, single-bit output and Orchestra-90 are
emulated but do not offload waveform scheduling.

**Mega Mini MPI/YMF262 OPL3 is real-hardware-only in this environment.** The user
owns it physically; installed MAME does not emulate that board's sound hardware.
No OPL3 probe, emulated result, or substitute chip topology was attempted. Keep
OPL3 as a separate future backend of the semantic audio interface.

Read alongside [audio archaeology and multitasking findings](DAGGORATH_AUDIO_RESEARCH.md),
[the source precedence index](../source-index/README.md),
[sound sources](../source-index/SOUND.md), and
[EOU transport inventory](../architecture/NITROS9_TRANSPORT_INVENTORY.md).
`MCP/Documents/` and `DOCS_INDEX.md` remain unavailable; primary manufacturer
material and identified implementation sources supply the hardware evidence.

## 1. What was actually verified

- Ran installed `-listslots`, `-listdevices`, `-listmedia`, `-listxml`,
  `-showconfig`, `-listmidi`, and an SSC ROM audit.
- Constructed discovery-only MPI variants with one candidate in empty slot 3,
  preserving SSC slot 2 and SCII + RTC slot 4. All four candidate device trees
  below were accepted.
- Inspected MAME **mame0289** device, MPI, MIDI output and chip implementations.
  Absence of a device's XML `feature` warning is not proof of completeness.
- Inspected targeted EOU files with read-only ToolShed exports and official
  upstream at `f470fa52eb172b59b22c1b722074998cb42de9b1`.
- Briefly started a **diskless**, audio-disabled MAME instance solely to enumerate
  MIDI image bindings, then exited normally. It used isolated cfg/nvram paths;
  no canonical media or save state was attached. This did not exercise synthesis.

[Exact discovery commands/results](assets/daggorath-audio-backends/discovery.json)
and [slot/device/media listings](assets/daggorath-audio-backends/mame-discovery.txt)
are retained. Large generated XML and downloaded source remain outside Git in
ignored `MCP/work/audio-backends/`.

### Verified topology

```text
coco3h -ramsize 2M
-ext multi
-ext:multi:slot1 ""
-ext:multi:slot2 ssc
-ext:multi:slot3 ""
-ext:multi:slot4 scii
-ext:multi:slot4:scii:meb rtime
```

RGB remains the existing `:screen_config=1` configuration. For discovery of a
candidate, replace **only** the empty slot-3 option with `gmc`, `ccpsg`, `midi`,
or `orch90`. These are isolated test variants, not changes to defaults. Each
needs its own boot/state baseline; do not load a canonical state across changed
hardware topology.

This installed CLI rejects dynamic `-ext ...` arguments with `-listxml` and
`-showconfig`. We therefore used plain `coco3h -listxml` for driver/default
configuration, device short names for device XML, and configured `-listdevices`
for the actual MPI tree. Device XML with clock 0/omitted cannot override the
configured device-clock listing. `coco_gmc -listxml` fails; the actual XML device
name is **`cocopakgmc`**, while its slot option is **`gmc`**.

## 2. Backend matrix

“Automatable” means a viable experiment using disposable guest artifacts and
existing command completion, plus the specified measurement harness. It does
not mean an audio assertion tool or production guest driver already exists.

| Backend | Exact slot / device identity | Installed implementation and offload | Coexistence with canonical MPI | Automated-test readiness / fidelity boundary |
|---|---|---|---|---|
| Tandy Speech/Sound | `ssc` / `coco_ssc`; canonical slot 2 | TMS7040 controller + AY-3-8913 + SP0256, RAM, actual ROMs and audio routes. **Yes:** chip waveforms and buffered controller playback | Already installed. Globally installed `$FF7D–$FF7E` handlers avoid SCS slot switching for command access. Audio mux/joystick ownership still required | **Best next experiment.** Firmware/protocol and sound output are implemented; calibrated WAV and scheduler measurements still needed. Analog activity detection and real CoCo 3 high-speed compatibility require separate validation |
| Game Master Cartridge | `gmc` / `cocopakgmc`; candidate slot 3 | SN76489A at 4 MHz with register writes and speaker route. **Yes:** tone/noise; host schedules sweeps, attenuation and mute | Accepted alongside SSC/SCII. `$FF41` writes require correct SCS selection; preserve disk slot and CTS. **Conflict with enabled global Becker interception** | **Practical second experiment**, after short MPI ownership transactions are verified. Simple chip/register tests are meaningful; physical audio routing, write spacing and analog sound remain hardware checks |
| CoCo PSG | `ccpsg` / `coco_psg`; candidate slot 3 | YM2149, register/control handlers, audio route. **Yes:** tone/noise/shared envelope | Accepted. SCS-gated `$FF5D–$FF5F`; slot arbitration required. Leave flash/SRAM/control features outside the sound experiment | **Functional tests practical; pitch calibration conditional.** Installed clock/source disagrees with board manual; do not use uncalibrated absolute-frequency results as hardware truth |
| Rutherford MIDI Pak | `midi` / `coco_midi`; candidate slot 3 | MC6850 + 500 kHz clock, MIDI IN/OUT/THRU serial devices. **UART offload only; no synthesizer in the Pak** | Accepted. Global `$FF6E–$FF6F` handlers; no SCS switching for these addresses. ACIA/CART interrupt ownership needs a compatible OS service | **Conditional:** host PortMidi support exists, but zero endpoints found. Add capture endpoint and guest binary-MIDI path. Protocol output can be tested independently of synth audio; receiver determines timbre/latency |
| DriveWire MIDI | Root `dwsock` / `coco_dwsock` (“Virtual Becker Port”), not a synthesizer cartridge | TCP transport implemented. Host DW server + chosen synth perform synthesis | Becker exists without an extra MPI slot, but defaults **Off**. `$FF41–$FF42` interception conflicts with GMC. Canonical guest has no verified active DW MIDI stack | **Infrastructure first.** Must validate guest transport/SCF modules, server, MIDI routing and capture before automatic audio tests |
| Original CoCo DAC / single bit | Root `dac` = 6-bit binary-weighted DAC; `sbs` = 1-bit DAC, with PIAs/routing | Audio devices implemented. **No autonomous sample/edge scheduling** | Built in. Shared audio mux and joystick/other audio interactions remain | Emulated output is measurable, but faithful foreground waveform loops are not a safe multitasking backend. Prior SS.Tone scheduling findings still apply |
| Orchestra-90 | `orch90` / `coco_orch90`; candidate slot 3 | Two 8-bit R-2R DACs and stereo route; writes `$FF7A/$FF7B`. **No waveform-clock offload** | Accepted; globally installed write handlers, distinct from SSC/MIDI addresses | Useful sample-output reference, **not a solution to CPU waveform load**. Requires a separately validated OS-compatible sample service |
| Mega Mini MPI OPL3 | **No installed MAME board/device topology** | Physical YMF262 provides FM/envelope offload | Physical Mega Mini topology/virtual slots must be inventoried separately | **Real-hardware-only future backend. No MAME audio validation available** |

The other relevant built-in output is floppy mechanical sound, not a Daggorath
synthesis backend. Generic MAME sound-chip support does not make an unattached
CoCo cartridge available.

## 3. EOU and upstream support: source is not an installed service

External upstream paths in this section are relative to
`/Volumes/SEDONA/Projects/nitros9-reference` at the commit above. Searches were
targeted by inventory, hardware addresses, device names and build conditionals;
“not found” is not a claim that no historical third-party driver exists anywhere.

| Backend | EOU evidence | Current upstream evidence / limitation |
|---|---|---|
| SSC | `/dd/SOURCECODE/ASM/NITROS9/SCF/sspak.asm`, Bruce Isted public-domain driver, 1987; module library `SCF/sspak.dr`, `SCF/ssp.dd` | `level1/coco1/modules/sspak.asm`, `ssp.asm`. **Text-to-speech only**; not a ready sound-buffer driver and not proof of Level II runtime installation |
| GMC | No dedicated GMC source identified in indexed EOU collection | `level1/wildbits/cmds/play.as` explicitly has non-WILDBITS CoCo+GMC code (`psgout`, hardware setup); `snddemo.asm` also writes the chip. Modern examples, **not an audited Level II service**. Setup selects MPI slot 1; do not copy that into a slot-3 test |
| CoCo PSG | No matching dedicated driver/example identified | No CoCo PSG YM2149 service identified. `level1/wildbits/libs/wbsnd/psg.as` is not sufficient evidence: chip/protocol/build target must match, not just the filename |
| MIDI Pak | `/dd/SOURCECODE/ASM/2_01_boot/sermidi` is a 106-byte program binary, CRC 5BE4AE, not evidence of a source driver for this ACIA | Generic `level1/modules/sc6850.asm`/`term_sc6850.asm` and `defs/midi.d` are references, not a verified drop-in MIDI Pak driver/descriptor |
| DW MIDI | `/dd/SOURCECODE/ASM/NITROS9/DW/midi`: descriptor `MIDI`, SCF + `scdwv`, 61 bytes, CRC 096D5F. EOU library also contains `midi_scdwv.dd`, scdwv and DWIO variants | `level1/modules/scdwvdesc.asm` names address 14 MIDI; `scdwv.asm`, `dwio.asm`, Becker read/write variants, `defs/midi.d`. This is concrete transport support; canonical boot availability is a separate question |
| DAC / single bit | `/dd/SOURCECODE/ASM/NITROS9/SCF/snddrv_beta6.asm`; `/dd/SOURCECODE/ASM/SOUNDRV/{SounDrv,DrvPlay}.asm`; `/dd/SOURCECODE/ASM/PLAY/play.a`; original Daggorath sound sources | `level2/coco3/modules/snddrv_cc3.asm` and VTIO countdown. Waveform generation still executes on CPU; no verified background faithful one-bit service identified |
| Orchestra-90 | `ASM/PLAY/play.a` has actual `$FF7A/$FF7B` sample writes in playback branches | No safe Level II autonomous playback service established. Historical masked-IRQ/direct-MMU playback is not the desired lifecycle |
| Mega Mini OPL3 | No verified OPL3 audio driver established by this pass | Mega Mini **UART DriveWire** routines exist; that is not an OPL3 driver |

The development VHD hash before and after targeted exports was identical:
`4c6bdc0cd2f68aa57a9c75f75f15fe429f30926aa522917dd8d14aa1b9ae622e`.
Exports used `os9 copy media/63SDC-MCP-DEV.VHD,<guest-path> <ignored-host-output>`;
no source was copied back into the image.

## 4. Speech/Sound: why it is a meaningful next target

Installed canonical `-listdevices` reports controller and AY clock approximately
1.78 MHz, SP0256 3.12 MHz, RAM, speaker and sound-activity filter. Device XML
names the two required firmware ROMs, and `coco_ssc -verifyroms` reports both
available as one good ROM set. The implementation connects host command writes
to the controller interrupt/latch, executes firmware, and connects the controller
bus to PSG/speech/RAM. This is substantially more than a slot stub. See
[MAME SSC implementation](https://github.com/mamedev/mame/blob/mame0289/src/devices/bus/coco/coco_ssc.cpp).

The [Tandy programming manual](https://tlindner.macmess.org/wp-content/uploads/2006/09/sscmanual.pdf),
pp10–13, 16–18, 24–26 and Appendix B, supplies buffered sound and PSG commands.
A future OS-9 client can submit a bounded sequence and yield while the Pak
synthesizes it: it need not generate audio-rate DAC samples. Command upload,
status polling, mux ownership and cancellation still consume CPU and need bounds.

Do **not** send arbitrary binary sound packets through the historical SSPak SCF
driver: `SWrite` strips bit 7 and filters characters. `BusyWait` is unbounded;
`SSWait` busy-polls speech onset before using F$Sleep. Its driver design is a
useful routing/provenance example, not our finished multitasking audio API.
A small owned test interface must preserve routing and avoid long polling loops.

MAME's sound-activity bit comes from an envelope/filter, not controller queue
completion. Silence inside an effect and startup delay invalidate it as the sole
completion fence. Verify a finite sequence with explicit mute, captured waveform,
known bounded deadline and responsive guest observer. Physical SSC clock/high-speed
compatibility and analog mux/filtering remain distinct checks; see the
[service material](https://tlindner.macmess.org/wp-content/uploads/2006/09/speechsoundcartridge.pdf)
and the existing audio research.

## 5. GMC and CoCo PSG: usable with explicit limits

[GMC source](https://github.com/mamedev/mame/blob/mame0289/src/devices/bus/coco/coco_gmc.cpp)
instantiates a 4 MHz SN76489A and routes offset-1 SCS writes to it. This supports
meaningful tone/noise/attenuation tests. It does not autonomously sequence a
whole effect or expose completion status. The chip can sustain sound while the
CPU sleeps; host updates and cleanup remain necessary.

[MPI source](https://github.com/mamedev/mame/blob/mame0289/src/devices/bus/coco/coco_multi.cpp)
routes SCS accesses only to the selected slot. A guest service must select the
sound slot only for a bounded transaction, preserve CTS/previous selection, and
coordinate with disk/RTC users. Do not leave the disk slot deselected while
sleeping. MAME routes GMC sound directly to a speaker, so successful emulator
output does not independently validate the physical CoCo cartridge audio mux.

[CoCo PSG source](https://github.com/mamedev/mame/blob/mame0289/src/devices/bus/coco/coco_psg.cpp)
implements YM2149 register access but instantiates **1 MHz** and toggles pin 26.
The [chip implementation](https://github.com/mamedev/mame/blob/mame0289/src/devices/sound/ay8910.cpp)
divides further with pin 26 low. This suggests effective 1 MHz/0.5 MHz operation,
where the [board manual](https://thezippsterzone.com/wp-content/uploads/2018/05/coco-psg-users-manual.pdf)
describes 2 MHz/1 MHz selection. Register semantics and offloaded operation are
useful test targets; **absolute pitch agreement is not established**. Measure a
known period and document the board revision before compensation or fidelity
claims. Do not modify flash or reuse whole control-register constants casually.

## 6. MIDI Pak: actual output, currently no receiver

The [Pak implementation](https://github.com/mamedev/mame/blob/mame0289/src/devices/bus/coco/coco_midi.cpp)
connects ACIA transmit to MIDI OUT and incoming serial data to both ACIA receive
and MIDI THRU. In the **verified slot-3 layout** the diskless enumeration found:

| CLI option | Image tag | Meaning |
|---|---|---|
| `-min` | `:ext:multi:slot3:midi:mdin:midiin:midiinimg` | MIDI input |
| `-mout1` | `:ext:multi:slot3:midi:mdthru:midiout:midioutimg` | **THRU** |
| `-mout2` | `:ext:multi:slot3:midi:mdout:midiout:midioutimg` | **OUT from guest ACIA** |

Do not infer numbering from the alphabetically printed device tree. Full device
tag as a CLI image option was rejected here; use the verified instance option.
An eventual invocation would add `-midiprovider pm -mout2 "<exact endpoint name>"`.

The output image's `call_load()` opens an **OS MIDI endpoint** named by its
argument; it is not a Standard MIDI File writer, despite `.mid` in `-listmedia`.
`rcv_complete()` forwards decoded bytes to that endpoint. Source:
[MIDI output image](https://github.com/mamedev/mame/blob/mame0289/src/devices/imagedev/midiout.cpp)
and [PortMidi provider](https://github.com/mamedev/mame/blob/mame0289/src/osd/modules/midi/portmidi.cpp).
The installed provider name **`pm`** is accepted. `portmidi` is not its CLI name
and falls back to auto. Both auto and explicitly selected `pm` reported
**“No MIDI ports were found”**, including outside the sandbox.

Proposed macOS verification, not configured in this task:

1. Create a named test destination through a CoreMIDI capture helper, or enable
   an IAC bus and attach a timestamping MIDI receiver. Apple documents IAC for
   inter-application routing in [Audio MIDI Setup](https://support.apple.com/en-il/guide/audio-midi-setup/ams1013/mac).
2. Re-run `-listmidi`; pass the exact output endpoint name to `-mout2`.
3. Capture bytes/messages, ordering, note-off/panic, and arrival timestamps.
   Assert these independently of waveform capture.
4. For audible tests, route to a pinned synth and soundbank; record that synth's
   audio separately. MAME's WAV output does not include an external synth.
5. Validate guest baud/framing, binary I/O and cancellation before game events.
   A matching MC6850 name alone does not supply the needed OS driver.

No endpoint was created and no notes sent. External synth state is not rewound
by MAME save states; reset owned notes on cancellation/epoch changes.

## 7. DriveWire MIDI: transport exists, complete path does not

Installed `coco3h` includes `dwsock`. Driver XML says **Becker Port Off** by
default; canonical generated cfg contains RGB but no Becker override. The
[source implementation](https://github.com/mamedev/mame/blob/mame0289/src/devices/bus/coco/coco_dwsock.cpp)
connects to **127.0.0.1:65504**, with selectable ports 65500–65509. The current
MCP does not start/manage a DW server. No listener was found at 65504 during
this pass; that does not establish whether a server is installed elsewhere.

The required path is:

```text
OS-9 binary /MIDI writes
  -> SCF + scdwv (virtual channel 14)
  -> compatible DWIO Becker transport
  -> enabled MAME Becker TCP endpoint
  -> DriveWire 4 server virtual-MIDI handling
  -> chosen server MIDI output / internal synth + bank
  -> MIDI capture and/or audio capture
```

The [DW4 GUI documentation](https://sourceforge.net/p/drivewireserver/wiki/The_DriveWire_GUI/)
describes selecting a MIDI output device, using its internal synthesizer, loading
a soundbank and MIDI translation configuration. Pin those settings and the server
version. A generic TCP sink is not a DW MIDI implementation, and a DW disk
connection alone does not verify virtual MIDI support.

EOU has the component files, but the canonical EMUHWCLK boot is not the DW boot
and `/MIDI` readiness was not established. A disposable boot must supply compatible
DWIO/scdwv/descriptor modules and preserve existing EmuDsk/Term behavior. MAME's
[CoCo driver](https://github.com/mamedev/mame/blob/mame0289/src/mame/trs/coco.cpp)
intercepts `$FF41–$FF42` before cartridge SCS dispatch when Becker is enabled:
**GMC at `$FF41` cannot be used simultaneously through that route.** This does
not affect the present disabled-Becker canonical layout.

Thus DW MIDI needs server lifecycle/configuration, guest stack integration,
transport health checks, a synth/capture target, latency tests and reset recovery
before it becomes an automated backend. It is not a ready substitute for SSC.

## 8. Automation and safety gates for the next experiment

Existing MCP tools can build/stage disposable artifacts, restore readiness,
execute commands and verify OS-9 status. **They do not assert audible output.**
`MCP/src/mame-process.ts:buildMameArgs` currently adds no `-wavwrite` or MIDI
endpoint option. Installed `-showconfig` does expose `wavwrite`.

For the next research experiment, use an isolated launcher/harness that supplies
`-wavwrite <private-output.wav>` to this MAME build while retaining the bridge.
That can be a test-only launch arrangement; this pass adds no public MCP tool or
source change. Then test one bounded SSC tone/noise sequence before adapting a
Daggorath event:

- ROM/topology identity, command acceptance, non-silent PCM, expected frequency
  band/duration and verified final silence; separate controller busy from sound
  activity and playback completion.
- Guest observer progress and scheduler gaps while synthesis runs; no audio-rate
  foreground waveform loop and no long interrupt masking.
- Cancellation, error, timeout and owned audio-route restoration, followed by a
  strict shell command.
- Repeatability with a fresh disposable-media boot. Record emulator/audio sample
  rate, gains and host configuration; exclude unrelated keyclick/floppy sounds
  using pre/post baselines and, later, separately measured output routes.
- Treat emulated protocol/waveform checks and physical electrical/analog checks
  as different evidence. Status 000 alone does not establish sound.

The [187 investigation](DAGGORATH_AUDIO_RESEARCH.md#15-status-187-follow-up-wizard-rejects-a-restored-wall-clock-jump)
showed RTC/calendar restore discontinuities. Do not use the Wizard's current
wall-clock subtraction as the audio benchmark clock, nor remove its guard to
force a pass. Use captured sample positions/emulated-time observations for
measurement and a verified cooperative guest timing contract. Device XML marks
the driver save-state support **unsupported**, despite our working practical
save/load workflow; validate each audio lifecycle and do not assume pending
controller events or external notes restore coherently.

## 9. Eventual real-hardware testing and Mega Mini OPL3

### Recommended hardware priorities

- **User-owned Mega Mini OPL3:** priority enhanced FM backend on the physical
  CoCo, once a safe OS-owned register/slot interface and transport exist.
- **SSC and GMC, if physical units are available:** best counterparts to the
  proposed MAME experiments; compare clock, analog routing, scheduler behavior
  and effect character. Do not assume the user owns these from emulator slots.
- **CoCo PSG:** valuable after oscillator/divider calibration on the actual board.
- **MIDI Pak / DW MIDI:** useful when a named synth/custom bank is part of the
  intended system, not as an interchangeable promise of original timbre.

### OPL3 remains an audio backend, not a transport

Keep semantic Daggorath events and gameplay completion independent of rendering.
A physical OPL3 service can later map events to FM patches, key-on/off, envelopes
and owned channels. Host automation transports commands/artifacts and observes
results; it does not substitute simulated OPL3 output for the actual board.

The [Mega Mini manual](https://thezippsterzone.com/wp-content/uploads/2018/12/MEGAmini-manual.pdf)
documents OPL3 virtual-slot access, register/data ports, audio routing and dual
UARTs. Before use, identify the user's board revision/configuration, slot map,
clock, cable interface and OS modules. The kernel-safe service must own selection,
preserve disk/UART mappings, respect register timing, and mute/release ownership
on cancellation. A raw board programming example is not an OS-9 interrupt API.

| Candidate transport | Useful role | Evidence and prerequisite |
|---|---|---|
| DriveWire | Deliver artifacts and carry a guest agent/control/status protocol | EOU/upstream DW support is real. Verify the chosen physical link, server, SCF virtual channel and completion protocol. Do not assume disk service automatically supplies a shell or agent |
| Mega Mini UART | Direct command/result stream or DW transport while OPL3 runs | Upstream `level1/modules/dwinit/dwinit_mmmpi.asm`, `dwread/dwread_mmmpi.asm`, `dwwrite/dwwrite_mmmpi.asm` exist. They select/restore MPI; the read path polls and can mask interrupts depending on build. Audit timeout, interrupt and slot coexistence before reuse; it is not a ready concurrent audio/control service |
| Serial / RS-232 | Simple bounded binary agent with host timestamps and acknowledgement | Verify actual UART/adapter, voltage/connector, baud, SCF driver and descriptor. Prefer a validated hardware UART path over assumptions about CPU-cost-free bitbanger serial |
| FujiNet | Possible artifact/network delivery component | Official [CoCo documentation](https://fujinetwifi.github.io/fujinet-docs/getting-started/coco-basics/) and [firmware configuration](https://github.com/FujiNetWIFI/fujinet-firmware/blob/master/platformio-sample.ini) establish CoCo DriveWire/Becker variants. They do **not** establish this EOU guest's remote agent, required virtual serial/MIDI services or board coexistence. Verify those explicitly; do not treat FujiNet as a drop-in DW4 MIDI server |

Minimum future physical test infrastructure:

1. Reproducible build/module provenance and a disposable staging destination.
2. A checksummed/length-framed guest agent protocol with request IDs, version and
   module identity, bounded completion/error reporting and cancellation.
3. A verified OS-9 transport and OPL3 service that permit another harmless process
   to progress. Review any IRQ lifecycle separately; no direct vector takeover.
4. Physical line-output capture through a known audio interface; record gain,
   sample rate and an event synchronization cue. Logic-analyzer register/bus traces
   are optional stronger timing evidence, not a requirement to begin PCM checks.
5. Hardware recovery/mute policy after disconnect or process failure. Real hardware
   has no MAME save-state rewind; reboot/reload/cleanup replaces that assumption.

No transport was configured, container deployed, IRQ installed, OPL3 code run,
or new audio backend implemented in this task.

## 10. Checks and remaining limits

This is a feasibility/discovery result, not a sound acceptance report. SSC/GMC/PSG
waveform and timing tests, macOS MIDI endpoint capture, DW end-to-end routing and
all physical-board tests remain future work. The diskless enumeration establishes
actual image bindings, not a MIDI send or an audible SSC result.

Only this document and curated discovery evidence were added for this task.
Canonical media, MCP/Daggorath implementation and external repositories were not
modified. No commit.

Validation: full MCP suite **115 passed, 0 failed**; `npm run build` and
`git diff --check` passed. Local asset links and explicit whitespace checks
(including untracked files) passed; curated listing trailing spaces were normalized.
All 15 previously recorded application/media fingerprints remained unchanged.
