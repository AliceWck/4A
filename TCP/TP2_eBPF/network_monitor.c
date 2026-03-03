#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>

SEC("xdp")
int monitor_traffic(struct xdp_md *ctx) {
    // Logique de filtrage ou de surveillance du trafic réseau
    bpf_printk("Packet received!\\n");
    return XDP_PASS; // Laisser passer le paquet
}

char _license[] SEC("license") = "GPL";