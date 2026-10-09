# Build outputs

The rebuild scripts place generated objects, listings, maps, and binaries under
`${TMPDIR:-/tmp}`.  Retained, byte-verified OS-9 modules are in `../artifacts/`;
the repository does not depend on files generated in this directory.
