#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0xaa, 0xfe),
	BT_DATA_BYTES(BT_DATA_SVC_DATA16, 0xaa, 0xfe, 0x10, 0x00, 0x03, /* Eddystone-URL https:// */
		      'z', 'e', 'p', 'h', 'y', 'r', 'p', 'r', 'o', 'j', 'e', 'c', 't', 0x08),
};
static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

int main(void)
{
	int err = bt_enable(NULL);
	if (err) {
		printk("bt_enable failed (%d)\n", err);
		return 0;
	}
	err = bt_le_adv_start(BT_LE_ADV_NCONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
	printk(err ? "advertising failed (%d)\n" : "Eddystone beacon advertising (%d)\n", err);
	k_sleep(K_FOREVER);
}
