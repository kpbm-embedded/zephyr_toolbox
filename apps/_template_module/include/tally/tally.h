#ifndef TALLY_TALLY_H_
#define TALLY_TALLY_H_

#include <zephyr/device.h>

typedef void (*tally_cb_t)(const struct device *dev, uint32_t count, void *user_data);

/* driver API; apps use the inline functions below */
struct tally_driver_api {
	int (*add)(const struct device *dev, uint32_t n);
	uint32_t (*get)(const struct device *dev);
	void (*reset)(const struct device *dev);
	void (*set_callback)(const struct device *dev, tally_cb_t cb, void *user_data);
};

static inline int tally_add(const struct device *dev, uint32_t n)
{
	return ((const struct tally_driver_api *)dev->api)->add(dev, n);
}

static inline uint32_t tally_get(const struct device *dev)
{
	return ((const struct tally_driver_api *)dev->api)->get(dev);
}

static inline void tally_reset(const struct device *dev)
{
	((const struct tally_driver_api *)dev->api)->reset(dev);
}

static inline void tally_set_callback(const struct device *dev, tally_cb_t cb, void *user_data)
{
	((const struct tally_driver_api *)dev->api)->set_callback(dev, cb, user_data);
}

#endif
