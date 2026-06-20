# Version Check Scripts

Scripts to verify if a container image version already exists in the registry before building.

## Scripts

### `check_version.sh`
Checks if a specific version tag exists in the container registry.

**Usage:**
```bash
./scripts/check_version.sh <version> [username] [token]
```

**Examples:**
```bash
# Check version 6.2.1 (anonymous access)
./scripts/check_version.sh 6.2.1

# Check with credentials
./scripts/check_version.sh 6.2.1 formica <token cli from registry>

# Using environment variables
export REGISTRY_USER=formica
export REGISTRY_TOKEN=<token cli from registry>
./scripts/check_version.sh 6.2.1
```

### `check_current_version.sh`
Automatically extracts the VERSION from `.gitlab-ci.yml` and checks the registry.

**Usage:**
```bash
./scripts/check_current_version.sh [username] [token]
```

**Examples:**
```bash
# Check current version from .gitlab-ci.yml
./scripts/check_current_version.sh

# With credentials
./scripts/check_current_version.sh formica <token cli from registry>
```

## Prerequisites

Install `skopeo`:
- **macOS**: `brew install skopeo`
- **Linux (RHEL/AlmaLinux)**: `dnf install skopeo`
- **Linux (Debian/Ubuntu)**: `apt-get install skopeo`

Install `jq` (optional, for prettier output):
- **macOS**: `brew install jq`
- **Linux**: `dnf install jq` or `apt-get install jq`

## Exit Codes

- `0`: Version is available (no collision)
- `1`: Version already exists (collision detected) or error occurred

## Integration with CI/CD

You can add this check to your GitLab CI pipeline:

```yaml
check-version:
  stage: lint
  image: registry.cern.ch/docker.io/library/fedora:latest
  before_script:
    - dnf install -y skopeo jq
  script:
    - ./scripts/check_current_version.sh
  only:
    - merge_requests
```

Or add it as a manual check before building:

```bash
# Check version before building
./scripts/check_current_version.sh || exit 1

# If OK, proceed with build
docker build -t local/crestapi:$(grep VERSION .gitlab-ci.yml | cut -d'"' -f2) .
```
