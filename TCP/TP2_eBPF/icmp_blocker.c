#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <linux/icmp.h>
#include <linux/ip.h>

SEC("xdp")
int icmp_blocker(struct xdp_md *ctx) {
    struct ethhdr *eth = (struct ethhdr *)(long)ctx->data;
    struct iphdr *iph = (struct iphdr *)(eth + 1);

    if (iph->protocol == IPPROTO_ICMP) {
        bpf_printk("ICMP packet dropped\\n");
        return XDP_DROP; // Bloquer le paquet ICMP
    }

    return XDP_PASS; // Laisser passer les autres paquets
}

char license[] SEC("license") = "GPL";