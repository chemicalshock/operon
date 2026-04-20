# Operon

Operon is a small C++ runtime for registering, describing, and executing algorithms through a common interface.

The project is intended to grow into a practical workbench for algorithm development: a place where new ideas can be built quickly, exercised with focused tests, and later promoted into stable native implementations. Over time, that workflow may include scripting or plugin-backed algorithms so prototypes can be explored without paying the full cost of wiring up production C++ on day one.

## Goals

- Provide a consistent runtime model for algorithms.
- Keep algorithm inputs and outputs represented with a shared value type.
- Make each algorithm testable on its own, without requiring the full runtime in every unit test.
- Support quick experiments while preserving a path toward compiled, production-quality implementations.
- Leave room for external registries, plugins, and scripting integrations.

## Shape Of The Project

Operon is organized around a few core concepts:

- `Algorithm` describes callable work, metadata, parameters, and return type.
- `Value` carries generic inputs and outputs across algorithm boundaries.
- `Result` represents success or structured failure.
- `Context` gives algorithms a place to receive execution state as the runtime grows.
- `Operon` owns registered algorithms and executes them by name.

The codebase currently favors direct, isolated unit tests for algorithms. Runtime-level tests should cover registration, lookup, dispatch, and integration behavior; algorithm tests should usually call the algorithm implementation directly.

## Building

Operon uses the repository makefile and the bundled `makeshift` dependency.

```sh
make
```

By default, the project builds a 64-bit debug configuration with the command-line target and shared library enabled.

## Testing

Unit tests live under `tst/ut` and use `shocktest`.

```sh
make ut
```

The unit-test makefile builds small SUT objects for the code under test, then links test binaries from those pieces. A SUT target can contain multiple source files when an algorithm is split across implementation units.

## Algorithm Development

The intended workflow for adding or experimenting with an algorithm is:

1. Define the algorithm metadata and callable function.
2. Keep the algorithm implementation independent of runtime registration where practical.
3. Add focused unit tests that call the algorithm directly.
4. Register the algorithm with the runtime once the standalone behavior is solid.
5. Add runtime-level coverage only for integration behavior.

That split keeps tests fast and specific while still allowing the runtime to provide a uniform execution surface for users and tools.

## Future Direction

Operon is expected to move toward a more exploratory development loop:

- lightweight algorithm prototypes,
- scripting-backed implementations,
- plugin-loaded registries,
- richer metadata for tooling and documentation,
- command-line workflows for trying algorithms interactively,
- simple promotion paths from prototype to compiled implementation.

The long-term aim is not just to collect algorithms, but to make it pleasant to invent, test, compare, and evolve them.
