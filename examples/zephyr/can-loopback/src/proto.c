#include <errno.h>
#include <zephyr/sys/byteorder.h>
#include "proto.h"

const struct can_filter proto_status_filter = {
	.id = PROTO_STATUS_BASE,
	.mask = CAN_STD_ID_MASK & ~0xf, /* ignore the low 4 bits: node number */
	.flags = 0,
};

void proto_encode(const struct node_status *s, struct can_frame *f)
{
	*f = (struct can_frame){.id = PROTO_STATUS_BASE + (s->node & 0xf), .dlc = 4};
	f->data[0] = s->state;
	f->data[1] = s->errors;
	sys_put_le16(s->mv, &f->data[2]);
}

int proto_decode(const struct can_frame *f, struct node_status *s)
{
	if ((f->flags & CAN_FRAME_IDE) || (f->id & ~0xfU) != PROTO_STATUS_BASE || f->dlc != 4) {
		return -EINVAL;
	}
	s->node = f->id & 0xf;
	s->state = f->data[0];
	s->errors = f->data[1];
	s->mv = sys_get_le16(&f->data[2]);
	return 0;
}
