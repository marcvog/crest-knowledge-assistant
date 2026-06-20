#!/bin/bash
################################################################################
# Script to check if a version tag already exists in the container registry
# Uses skopeo to query the registry for existing tags
################################################################################

set -e

# Configuration
REGISTRY="registry.cern.ch/crest/crestapi"
VERSION="${1:-}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to display usage
usage() {
    echo "Usage: $0 <version> [username] [token]"
    echo ""
    echo "Arguments:"
    echo "  version    : Version to check (e.g., 6.2.1)"
    echo "  username   : Registry username (optional, uses env REGISTRY_USER if not provided)"
    echo "  token      : Registry token (optional, uses env REGISTRY_TOKEN if not provided)"
    echo ""
    echo "Environment variables:"
    echo "  REGISTRY_USER  : Username for registry authentication"
    echo "  REGISTRY_TOKEN : Token/password for registry authentication"
    echo ""
    echo "Example:"
    echo "  $0 6.2.1"
    echo "  $0 6.2.1 formica pBozvbA5ZZGPhwzfN0bgpw8GdjOGUtPu"
    echo "  REGISTRY_USER=formica REGISTRY_TOKEN=xxx $0 6.2.1"
    exit 1
}

# Check if version is provided
if [ -z "$VERSION" ]; then
    echo -e "${RED}Error: Version number is required${NC}"
    usage
fi

# Get credentials from arguments or environment variables
USERNAME="${2:-${REGISTRY_USER:-}}"
TOKEN="${3:-${REGISTRY_TOKEN:-}}"

# Check if skopeo is installed
if ! command -v skopeo &> /dev/null; then
    echo -e "${RED}Error: skopeo is not installed${NC}"
    echo "Please install skopeo:"
    echo "  - macOS: brew install skopeo"
    echo "  - Linux: dnf install skopeo (or apt-get install skopeo)"
    exit 1
fi

# Build skopeo command
SKOPEO_CMD="skopeo list-tags"
if [ -n "$USERNAME" ] && [ -n "$TOKEN" ]; then
    SKOPEO_CMD="$SKOPEO_CMD --creds ${USERNAME}:${TOKEN}"
fi
SKOPEO_CMD="$SKOPEO_CMD docker://${REGISTRY}"

echo "Checking registry for existing tags..."
echo "Registry: ${REGISTRY}"
echo "Version to check: ${VERSION}"
echo ""

# Query the registry
RESULT=$(eval "$SKOPEO_CMD" 2>&1)
SKOPEO_EXIT_CODE=$?

if [ $SKOPEO_EXIT_CODE -ne 0 ]; then
    echo -e "${RED}Error querying registry:${NC}"
    echo "$RESULT"
    exit 1
fi

# Parse the result and check if version exists
echo "Available tags:"
echo "$RESULT" | jq -r '.Tags[]' 2>/dev/null || echo "$RESULT"
echo ""

# Check if the version exists in the tags
if echo "$RESULT" | grep -q "\"${VERSION}\""; then
    echo -e "${RED}❌ COLLISION DETECTED!${NC}"
    echo -e "${RED}Version ${VERSION} already exists in the registry.${NC}"
    echo ""
    echo "Please choose a different version number."
    exit 1
else
    echo -e "${GREEN}✅ OK - Version ${VERSION} is available${NC}"
    echo -e "${GREEN}You can safely use this version.${NC}"
    exit 0
fi
