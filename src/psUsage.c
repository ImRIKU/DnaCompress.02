#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
    unsigned long iowait;
    unsigned long irq;
    unsigned long softirq;
} CPUUsage;

void get_cpu_usage(CPUUsage* usage) {
    FILE* file = fopen("/proc/stat", "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "cpu  %lu %lu %lu %lu %lu %lu %lu",
           &usage->user,
           &usage->nice,
           &usage->system,
           &usage->idle,
           &usage->iowait,
           &usage->irq,
           &usage->softirq);

    fclose(file);
}

unsigned long get_total_time(CPUUsage* usage) {
    return usage->user + usage->nice + usage->system + usage->idle + 
           usage->iowait + usage->irq + usage->softirq;
}

float calculate_cpu_usage(CPUUsage* prev, CPUUsage* curr) {
    unsigned long prev_total = get_total_time(prev);
    unsigned long curr_total = get_total_time(curr);

    unsigned long total_diff = curr_total - prev_total;
    unsigned long idle_diff = curr->idle - prev->idle;

    return (1.0 - ((float)idle_diff / total_diff)) * 100;
}

void get_memory_usage(unsigned long* total, unsigned long* free) {
    FILE* file = fopen("/proc/meminfo", "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        if (sscanf(buffer, "MemTotal: %lu kB", total) == 1 ||
            sscanf(buffer, "MemFree: %lu kB", free) == 1) {
            // Do nothing, just parsing
        }
    }

    fclose(file);
}

int main() {
    CPUUsage prev_usage, curr_usage;
    unsigned long mem_total, mem_free;

    // Get initial CPU usage
    get_cpu_usage(&prev_usage);
    sleep(1); // Wait for a second
    // Get CPU usage after 1 second
    get_cpu_usage(&curr_usage);

    // Calculate CPU usage percentage
    float cpu_usage = calculate_cpu_usage(&prev_usage, &curr_usage);
    printf("CPU Usage: %.2f%%\n", cpu_usage);

    // Get memory usage
    get_memory_usage(&mem_total, &mem_free);
    unsigned long mem_used = mem_total - mem_free;
    printf("Memory Usage: %lu kB used out of %lu kB\n", mem_used, mem_total);

    return 0;
}