# Debian Bookworm = same base as the Raspberry Pi 5 image (GCC 12, glibc 2.36).
FROM mcr.microsoft.com/devcontainers/base:bookworm

ARG TZ=Europe/Berlin
ARG LLVM_VERSION=23
ARG CMAKE_VERSION=4.4.4
ARG CLAUDE_CODE_VERSION=latest
ENV TZ=${TZ}

# Compiler, build tools, vcpkg prerequisites and firewall tooling.
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get install -y --no-install-recommends \
        build-essential g++ gdb ninja-build ccache pkg-config \
        ca-certificates curl gnupg git zip unzip tar xz-utils \
        python3 autoconf autoconf-archive automake libtool nasm bison flex \
        iptables ipset iproute2 dnsutils aggregate jq \
        less procps fzf \
    && rm -rf /var/lib/apt/lists/*

# clangd / clang-tidy / clang-format from apt.llvm.org (only the tooling; GCC stays the compiler).
RUN curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor -o /usr/share/keyrings/llvm.gpg \
    && echo "deb [signed-by=/usr/share/keyrings/llvm.gpg] http://apt.llvm.org/bookworm/ llvm-toolchain-bookworm-${LLVM_VERSION} main" \
        > /etc/apt/sources.list.d/llvm.list \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
        clangd-${LLVM_VERSION} clang-tidy-${LLVM_VERSION} clang-format-${LLVM_VERSION} \
    && rm -rf /var/lib/apt/lists/* \
    && for t in clangd clang-tidy clang-format run-clang-tidy; do \
           if [ -e /usr/bin/$t-${LLVM_VERSION} ]; then ln -sf /usr/bin/$t-${LLVM_VERSION} /usr/local/bin/$t; fi; \
       done

# Recent CMake (Bookworm ships 3.25), checksum-verified.
RUN ARCH=$(uname -m) && cd /tmp \
    && curl -fsSLO https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-${ARCH}.sh \
    && curl -fsSL https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-SHA-256.txt \
        | grep "cmake-${CMAKE_VERSION}-linux-${ARCH}.sh" | sha256sum -c - \
    && sh cmake-${CMAKE_VERSION}-linux-${ARCH}.sh --skip-license --prefix=/usr/local --exclude-subdir \
    && rm cmake-${CMAKE_VERSION}-linux-${ARCH}.sh

# vcpkg (full clone: manifest mode resolves "builtin-baseline" from git history).
ENV VCPKG_ROOT=/opt/vcpkg \
    VCPKG_DISABLE_METRICS=1
ENV PATH=${VCPKG_ROOT}:${PATH}
RUN git clone https://github.com/microsoft/vcpkg.git ${VCPKG_ROOT} \
    && ${VCPKG_ROOT}/bootstrap-vcpkg.sh -disableMetrics \
    && chown -R vscode:vscode ${VCPKG_ROOT}

# Cache dirs live on a named volume (vcpkg binary cache + ccache survive rebuilds).
RUN mkdir -p /home/vscode/.cache/vcpkg/downloads /home/vscode/.cache/vcpkg/archives \
             /home/vscode/.cache/ccache /home/vscode/.claude \
    && chown -R vscode:vscode /home/vscode/.cache /home/vscode/.claude

# Firewall. The base image gives "vscode" passwordless sudo for everything, which would let
# anything running in the container flush the firewall again. Restrict sudo to this one script.
# Installed under its own name so nothing else that ships an init-firewall.sh can replace it.
COPY init-firewall.sh allowed-domains.txt /usr/local/etc/claude-firewall/
RUN mv /usr/local/etc/claude-firewall/init-firewall.sh /usr/local/sbin/devcontainer-firewall.sh \
    && chmod 0755 /usr/local/sbin/devcontainer-firewall.sh \
    && chmod 0644 /usr/local/etc/claude-firewall/allowed-domains.txt \
    && rm -f /etc/sudoers.d/vscode \
    && echo "vscode ALL=(root) NOPASSWD: /usr/local/sbin/devcontainer-firewall.sh" > /etc/sudoers.d/vscode-firewall \
    && chmod 0440 /etc/sudoers.d/vscode-firewall

ENV DEVCONTAINER=true
USER vscode

# Claude Code via Anthropic's native installer (no Node.js needed). It lives in ~/.local and
# updates itself from downloads.claude.ai, which is on the firewall allowlist.
ENV PATH=/home/vscode/.local/bin:${PATH}
RUN curl -fsSL https://claude.ai/install.sh | bash -s ${CLAUDE_CODE_VERSION} \
    && claude --version

WORKDIR /workspace
