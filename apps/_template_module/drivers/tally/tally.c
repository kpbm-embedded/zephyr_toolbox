#define DT_DRV_COMPAT acme_tally

#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/spinlock.h>
#include <tally/tally.h>

struct tally_config {
	uint32_t threshold;
	uint32_t initial;
};

struct tally_data {
	struct k_spinlock lock;
	uint32_t count;
	tally_cb_t cb;
	void *user_data;
};

static int tally_add_impl(const struct device *dev, uint32_t n)
{
	const struct tally_config *cfg = dev->config;
	struct tally_data *data = dev->data;
	k_spinlock_key_t key = k_spin_lock(&data->lock);
	uint32_t before = data->count, after;
	tally_cb_t cb = data->cb;

	if (n > UINT32_MAX - before) {
		k_spin_unlock(&data->lock, key);
		return -EOVERFLOW;
	}
	after = data->count = before + n;
	k_spin_unlock(&data->lock, key);
	if (cb && before < cfg->threshold && after >= cfg->threshold) {
		cb(dev, after, data->user_data); /* outside the lock */
	}
	return 0;
}

static uint32_t tally_get_impl(const struct device *dev)
{
	return ((struct tally_data *)dev->data)->count;
}

static void tally_reset_impl(const struct device *dev)
{
	const struct tally_config *cfg = dev->config;
	struct tally_data *data = dev->data;
	k_spinlock_key_t key = k_spin_lock(&data->lock);

	data->count = cfg->initial;
	k_spin_unlock(&data->lock, key);
}

static void tally_set_callback_impl(const struct device *dev, tally_cb_t cb, void *user_data)
{
	struct tally_data *data = dev->data;

	data->cb = cb;
	data->user_data = user_data;
}

static const struct tally_driver_api tally_api = {
	.add = tally_add_impl,
	.get = tally_get_impl,
	.reset = tally_reset_impl,
	.set_callback = tally_set_callback_impl,
};

static int tally_init(const struct device *dev)
{
	tally_reset_impl(dev);
	return 0;
}

#define TALLY_DEFINE(inst)                                                                         \
	static struct tally_data tally_data_##inst;                                                \
	static const struct tally_config tally_config_##inst = {                                   \
		.threshold = DT_INST_PROP(inst, threshold),                                        \
		.initial = DT_INST_PROP(inst, initial),                                            \
	};                                                                                         \
	DEVICE_DT_INST_DEFINE(inst, tally_init, NULL, &tally_data_##inst, &tally_config_##inst,    \
			      POST_KERNEL, CONFIG_TALLY_INIT_PRIORITY, &tally_api);

DT_INST_FOREACH_STATUS_OKAY(TALLY_DEFINE)
