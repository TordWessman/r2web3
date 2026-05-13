# r2common

Shared Arduino library for ESP8266, ESP32, and AtmelSAM platforms. Provides common utilities (logging, result types, network abstractions) used across r2 projects.

## Git Subtree Usage

Projects that depend on `r2common` should include it as a git subtree.

### One-time setup (add r2common to your project)

```bash
git subtree add --prefix=src/r2common https://github.com/TordWessman/r2common.git main --squash
```

### Pull updates from r2common into your project

```bash
git subtree pull --prefix=src/r2common https://github.com/TordWessman/r2common.git main --squash
```

### Push fixes made in your project back to r2common

```bash
git subtree push --prefix=src/r2common https://github.com/TordWessman/r2common.git main
```
