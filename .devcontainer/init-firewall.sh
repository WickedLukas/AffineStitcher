#!/bin/bash
# Default-deny egress firewall for the dev container.
# Based on Anthropic's reference: https://github.com/anthropics/claude-code/tree/main/.devcontainer
# Differences: domains come from allowed-domains.txt, no blanket outbound SSH, IPv6 is blocked,
# and unresolvable domains are reported instead of aborting.
set -euo pipefail
IFS=$'\n\t'

DOMAINS_FILE=/usr/local/etc/claude-firewall/allowed-domains.txt

# Reset default policies so the script can be re-run while the firewall is active.
iptables -P INPUT ACCEPT
iptables -P FORWARD ACCEPT
iptables -P OUTPUT ACCEPT

# Keep Docker's embedded DNS (127.0.0.11) working after the flush.
DOCKER_DNS_RULES=$(iptables-save -t nat | grep "127\.0\.0\.11" || true)

iptables -F
iptables -X
iptables -t nat -F
iptables -t nat -X
iptables -t mangle -F
iptables -t mangle -X
ipset destroy allowed-domains 2>/dev/null || true

if [ -n "$DOCKER_DNS_RULES" ]; then
    iptables -t nat -N DOCKER_OUTPUT 2>/dev/null || true
    iptables -t nat -N DOCKER_POSTROUTING 2>/dev/null || true
    echo "$DOCKER_DNS_RULES" | xargs -L 1 iptables -t nat
fi

# DNS and loopback.
iptables -A OUTPUT -p udp --dport 53 -j ACCEPT
iptables -A INPUT -p udp --sport 53 -j ACCEPT
iptables -A INPUT -i lo -j ACCEPT
iptables -A OUTPUT -o lo -j ACCEPT

ipset create allowed-domains hash:net

echo "Fetching GitHub IP ranges..."
gh_ranges=$(curl -fsS --connect-timeout 10 https://api.github.com/meta)
if ! echo "$gh_ranges" | jq -e '.web and .api and .git' >/dev/null; then
    echo "ERROR: GitHub meta response missing required fields"
    exit 1
fi
while read -r cidr; do
    if [[ ! "$cidr" =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}/[0-9]{1,2}$ ]]; then
        echo "ERROR: invalid CIDR from GitHub meta: $cidr"
        exit 1
    fi
    ipset add -exist allowed-domains "$cidr"
done < <(echo "$gh_ranges" | jq -r '(.web + .api + .git)[]' | grep -v ':' | aggregate -q)

while read -r domain; do
    ips=$(dig +noall +answer A "$domain" | awk '$4 == "A" {print $5}')
    if [ -z "$ips" ]; then
        echo "WARNING: could not resolve $domain, skipping"
        continue
    fi
    while read -r ip; do
        if [[ ! "$ip" =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
            echo "ERROR: invalid IP for $domain: $ip"
            exit 1
        fi
        ipset add -exist allowed-domains "$ip"
    done <<< "$ips"
    echo "Allowed $domain"
done < <(grep -vE '^\s*(#|$)' "$DOMAINS_FILE")

# Docker bridge network (VS Code server <-> host communication).
HOST_IP=$(ip route | grep default | cut -d" " -f3)
if [ -z "$HOST_IP" ]; then
    echo "ERROR: failed to detect host IP"
    exit 1
fi
HOST_NETWORK=$(echo "$HOST_IP" | sed "s/\.[0-9]*$/.0\/24/")
iptables -A INPUT -s "$HOST_NETWORK" -j ACCEPT
iptables -A OUTPUT -d "$HOST_NETWORK" -j ACCEPT

iptables -P INPUT DROP
iptables -P FORWARD DROP
iptables -P OUTPUT DROP
iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -A OUTPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -A OUTPUT -m set --match-set allowed-domains dst -j ACCEPT
iptables -A OUTPUT -j REJECT --reject-with icmp-admin-prohibited

# No IPv6 egress at all (the allowlist above is IPv4 only).
if ip6tables -L >/dev/null 2>&1; then
    ip6tables -F
    ip6tables -A INPUT -i lo -j ACCEPT
    ip6tables -A OUTPUT -o lo -j ACCEPT
    ip6tables -P INPUT DROP
    ip6tables -P FORWARD DROP
    ip6tables -P OUTPUT DROP
fi

echo "Firewall configured, verifying..."
if curl -fsS --connect-timeout 5 https://example.com >/dev/null 2>&1; then
    echo "ERROR: firewall verification failed - example.com is reachable"
    exit 1
fi
if ! curl -sS --connect-timeout 5 -o /dev/null https://api.anthropic.com; then
    echo "ERROR: firewall verification failed - api.anthropic.com is not reachable"
    exit 1
fi
if ! curl -fsS --connect-timeout 5 -o /dev/null https://api.github.com/zen; then
    echo "ERROR: firewall verification failed - api.github.com is not reachable"
    exit 1
fi
echo "Firewall verification passed"
