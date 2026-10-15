from pathlib import Path
import sys
p = Path(sys.argv[1])
s = p.read_text()
needle = "                         pc = cpu.state.PC.value })\nend)\ntapInstalled = true"
assert s.count(needle) == 1, 'PB1 observer hook boundary missing or ambiguous'
replacement = """                         pc = cpu.state.PC.value })
    if currentEdge.generation == 6 then
        local ok, result = pcall(private_e13d_arm, currentEdge.generation, currentEdge.phase)
        if not ok then result = { error = tostring(result) } end
        record('trace_arm', result)
        if result.error then
            failed = 'generation-6 tracer arming failed: ' .. result.error
            record('failure', { reason = failed })
        end
    end
end)
tapInstalled = true"""
s = s.replace(needle, replacement)
s += '\n-- private Stage 3E.13D.1 timing tracer follows\n' + Path('/private/tmp/c6-stage3e13d/diagnostic.lua').read_text() + '\n'
p.write_text(s)
print(p)
