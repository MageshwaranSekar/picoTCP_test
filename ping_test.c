#include "pico_stack.h"
#include "pico_ipv4.h"
#include "pico_icmp4.h"
#include "pico_dev_tap.h"
#include "custom_memalloc.h"

#define NUM_PING 10
#define HEAPSIZE 200000
static int finished = 0;
static char ch[HEAPSIZE];
/* gets called when the ping receives a reply, or encounters a problem */
void cb_ping(struct pico_icmp4_stats *s)
{
    char host[30];
    pico_ipv4_to_string(host, s->dst.addr);
    if (s->err == 0) {
        /* if all is well, print some pretty info */
        printf("%lu bytes from %s: icmp_req=%lu ttl=%lu time=%lu ms\n", s->size,
                host, s->seq, s->ttl, (long unsigned int)s->time);
        if (s->seq >= NUM_PING)
            finished = 1;
    } else {
        /* if something went wrong, print it and signal we want to stop */
        printf("PING %lu to %s: Error %d\n", s->seq, host, s->err);
        finished = 1;
    }
}


int main(void){
    int id;
    struct pico_ip4 ipaddr, netmask;
    struct pico_device* dev;
		init_heap(ch, HEAPSIZE);
		
    /* initialise the stack. Super important if you don't want ugly stuff like
     * segfaults and such! */
    pico_stack_init();
		
    /* create the tap device */
    dev = pico_tap_create("tap0");
    if (!dev)
        return -1;

    /* assign the IP address to the tap interface */
    pico_string_to_ipv4("10.0.0.2", &ipaddr.addr);
    pico_string_to_ipv4("255.255.255.0", &netmask.addr);
    pico_ipv4_link_add(dev, ipaddr, netmask);

    
    //id = pico_icmp4_ping("10.0.0.1", NUM_PING, 1000, 10000, 64, cb_ping);
		printf("starting ping\n");
    if (id == -1)
        return -1;


    //while (finished != 1)
		while(1){
				pico_stack_tick();
				PICO_IDLE();
    }

    printf("finished !\n");
    return 0;
}
