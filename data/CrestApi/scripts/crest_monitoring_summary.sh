#!/usr/bin/env bash
# crest_monitoring_summary.sh
# Query /monitoring/payloads and print ranked summaries:
#   - top N tags by total volume
#   - top N tags by number of IOVs
#   - top N tags by average payload size
#
# Usage:
#   ./crest_monitoring_summary.sh [TAG_PATTERN] [BASE_URL] [TOP_N]
#
# Examples:
#   ./crest_monitoring_summary.sh 'UPGRADE%'
#   ./crest_monitoring_summary.sh '%' https://atlas-crest-dev.cern.ch/api-v6.0 20

TAG_PATTERN="${1:-%}"
BASE_URL="${2:-https://atlas-crest-dev.cern.ch/api-v6.0}"
TOP_N="${3:-10}"

URL="${BASE_URL}/monitoring/payloads?tagname=$(python3 -c "import urllib.parse,sys; print(urllib.parse.quote(sys.argv[1]))" "${TAG_PATTERN}")&size=100000"

echo "=================================================="
echo " CREST Monitoring Summary"
echo "  Base URL   : ${BASE_URL}"
echo "  Tag pattern: ${TAG_PATTERN}"
echo "  Top N      : ${TOP_N}"
echo "=================================================="
echo ""
echo "Fetching data from: ${URL}"

RAW=$(curl -sf "${URL}")
if [ $? -ne 0 ] || [ -z "${RAW}" ]; then
  echo "ERROR: Failed to fetch data from CREST." >&2
  exit 1
fi

TOTAL=$(echo "${RAW}" | jq '.resources | length')
echo "Total tags returned: ${TOTAL}"
echo ""

# Helper: human-readable bytes
hr_bytes() {
  python3 -c "
v=float('${1}')
for u in ['B','KB','MB','GB']:
    if v < 1024: print(f'{v:.1f} {u}'); break
    v /= 1024
else: print(f'{v:.1f} TB')
"
}

# --- Top N by total volume ---
echo "=================================================="
echo " TOP ${TOP_N} tags by TOTAL VOLUME"
echo "=================================================="
printf "%-6s %-70s %15s %15s\n" "Rank" "Tag Name" "Total Vol" "Avg Vol"
printf "%-6s %-70s %15s %15s\n" "----" "--------" "---------" "-------"

echo "${RAW}" | jq -r '
  .resources
  | sort_by(-.totvolume)
  | to_entries[]
  | [(.key+1), .value.tagname, .value.totvolume, .value.avgvolume, .value.niovs]
  | @tsv
' | head -"${TOP_N}" | while IFS=$'\t' read -r rank tagname totvol avgvol niovs; do
  printf "%-6s %-70s %15s %15s\n" \
    "${rank}" "${tagname}" "$(hr_bytes ${totvol})" "$(hr_bytes ${avgvol})"
done

echo ""

# --- Top N by number of IOVs ---
echo "=================================================="
echo " TOP ${TOP_N} tags by NUMBER OF IOVs"
echo "=================================================="
printf "%-6s %-70s %10s %15s %15s\n" "Rank" "Tag Name" "# IOVs" "Total Vol" "Avg Vol"
printf "%-6s %-70s %10s %15s %15s\n" "----" "--------" "------" "---------" "-------"

echo "${RAW}" | jq -r '
  .resources
  | sort_by(-.niovs)
  | to_entries[]
  | [(.key+1), .value.tagname, .value.niovs, .value.totvolume, .value.avgvolume]
  | @tsv
' | head -"${TOP_N}" | while IFS=$'\t' read -r rank tagname niovs totvol avgvol; do
  printf "%-6s %-70s %10s %15s %15s\n" \
    "${rank}" "${tagname}" "${niovs}" "$(hr_bytes ${totvol})" "$(hr_bytes ${avgvol})"
done

echo ""

# --- Top N by average payload size ---
echo "=================================================="
echo " TOP ${TOP_N} tags by AVERAGE PAYLOAD SIZE"
echo "=================================================="
printf "%-6s %-70s %10s %15s\n" "Rank" "Tag Name" "# IOVs" "Avg Vol"
printf "%-6s %-70s %10s %15s\n" "----" "--------" "------" "-------"

echo "${RAW}" | jq -r '
  .resources
  | sort_by(-.avgvolume)
  | to_entries[]
  | [(.key+1), .value.tagname, .value.niovs, .value.avgvolume]
  | @tsv
' | head -"${TOP_N}" | while IFS=$'\t' read -r rank tagname niovs avgvol; do
  printf "%-6s %-70s %10s %15s\n" \
    "${rank}" "${tagname}" "${niovs}" "$(hr_bytes ${avgvol})"
done

echo ""

# --- Overall stats ---
echo "=================================================="
echo " OVERALL STATS"
echo "=================================================="
echo "${RAW}" | jq -r '
  .resources |
  {
    total_tags:    length,
    total_iovs:    (map(.niovs) | add),
    total_volume:  (map(.totvolume) | add),
    max_niovs:     (max_by(.niovs)  | [.tagname, .niovs]),
    max_totvol:    (max_by(.totvolume) | [.tagname, .totvolume]),
    max_avgvol:    (max_by(.avgvolume) | [.tagname, .avgvolume])
  }
' | python3 -c "
import sys, json
d = json.load(sys.stdin)

def hr(v):
    for u in ['B','KB','MB','GB','TB']:
        if v < 1024: return f'{v:.1f} {u}'
        v /= 1024
    return f'{v:.1f} PB'

print(f\"  Total tags         : {d['total_tags']}\")
print(f\"  Total IOVs         : {d['total_iovs']}\")
print(f\"  Total volume       : {hr(d['total_volume'])}\")
print(f\"  Most IOVs          : {d['max_niovs'][0]}  ({d['max_niovs'][1]} IOVs)\")
print(f\"  Largest total vol  : {d['max_totvol'][0]}  ({hr(d['max_totvol'][1])})\")
print(f\"  Largest avg payload: {d['max_avgvol'][0]}  ({hr(d['max_avgvol'][1])})\")
"
echo "=================================================="
