#!/bin/bash
################################################################################
# Script to check if the current VERSION from .gitlab-ci.yml exists in registry
# This automatically extracts the version and runs the check
################################################################################

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
GITLAB_CI_FILE="${PROJECT_ROOT}/.gitlab-ci.yml"

# Colors for output
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if .gitlab-ci.yml exists
if [ ! -f "$GITLAB_CI_FILE" ]; then
    echo "Error: .gitlab-ci.yml not found at $GITLAB_CI_FILE"
    exit 1
fi

# Extract VERSION from .gitlab-ci.yml
VERSION=$(grep -E '^\s*VERSION:\s*"' "$GITLAB_CI_FILE" | head -1 | sed -E 's/.*"([^"]+)".*/\1/')

if [ -z "$VERSION" ]; then
    echo "Error: Could not extract VERSION from .gitlab-ci.yml"
    exit 1
fi

echo -e "${YELLOW}Extracted version from .gitlab-ci.yml: ${VERSION}${NC}"
echo ""

# Call the main check script
exec "${SCRIPT_DIR}/check_version.sh" "$VERSION" "$@"
