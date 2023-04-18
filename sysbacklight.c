#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <sysfs/libsysfs.h>

enum OP {
    QUERY,
    SET,
    INC,
    DEC
};

struct sysfs_class_device *find_brightness_device(void)
{
    struct sysfs_class *cls;
    struct dlist *backlights;

    cls = sysfs_open_class("backlight");
    if(!cls)
    {
        perror("Could not open backlight class");
        return NULL;
    }

    backlights = sysfs_get_class_devices(cls);
    if(!backlights)
    {
        perror("Could not enumerate backlight devices");
        return NULL;
    }

    struct sysfs_class_device *iter=NULL;
    struct sysfs_attribute *attr=NULL;

    dlist_for_each_data(backlights, iter, struct sysfs_class_device)
    {
        if((attr=sysfs_get_classdev_attr(iter, "type")))
        {
            if(!strncmp(attr->value, "raw", 3))
                return iter;
        }
    }

    return NULL;
}

double get_brightness(struct sysfs_class_device *dev)
{
    struct sysfs_attribute *attr;
    double brightness=0;
    double max_brightness=0;

    if((attr=sysfs_get_classdev_attr(dev, "max_brightness")))
    {
        max_brightness = strtod(attr->value, NULL);
    }
    if((attr=sysfs_get_classdev_attr(dev, "brightness")))
    {
        brightness = strtod(attr->value, NULL);
    }

    return (brightness / max_brightness) * 100;
}

bool set_brightness(struct sysfs_class_device *dev, enum OP op, double value)
{
    struct sysfs_attribute *attr;
    double brightness=0;
    double max_brightness=0;

    if((attr=sysfs_get_classdev_attr(dev, "max_brightness")))
    {
        max_brightness = strtod(attr->value, NULL);
    }

    if((attr=sysfs_get_classdev_attr(dev, "brightness")))
    {
        brightness = strtod(attr->value, NULL);
    }

    if(max_brightness < 1 || isnan(max_brightness))
    {
        fprintf(stderr, "Failed to acquire maximum brightness.");
        return false;
    }

    switch(op)
    {
        case SET:
            brightness = (max_brightness / 100) * value;
            break;

        case INC:
            brightness += (max_brightness / 100) * value;
            break;

        case DEC:
            brightness -= (max_brightness / 100) * value;
            break;

        default:
            // This is not supposed to happen
            return false;
    }

    if(brightness > max_brightness)
        return true;
    if(brightness < 0)
        return true;

    int len;
    char valbuf[32];
    len=snprintf(valbuf, 32, "%.0f", brightness);
    if(sysfs_write_attribute(attr, valbuf, len) != 0)
    {
        perror("Failed to write brightness attribute");
        return false;
    }

    return true;
}

int main(int argc, char **argv)
{
    struct sysfs_class_device *dev;

    enum OP op=QUERY;
    double value=0;

    dev = find_brightness_device();
    if(!dev)
    {
        fprintf(stderr, "Could not find backlight device of type raw!\n");
        return 1;
    }

    if(argc >= 2)
    {
        switch(argv[1][0])
        {
            case '?':
                op = QUERY;
                break;

            case '=':
                op = SET;
                break;

            case '+':
                op = INC;
                break;

            case '-':
                op = DEC;
                break;

            default:
                fprintf(stderr, "Unknown operation '%c'!\n", argv[1][0]);
                return 2;
        }

        if(op != QUERY)
        {
            if(argc < 3)
            {
                fprintf(stderr, "Operation '%c' needs a value argument!\n", argv[1][0]);
                return 2;
            }
            value = strtod(argv[2], NULL);
        }

        if(value < 0 || isnan(value))
        {
            fprintf(stderr, "Invalid brightness value\n");
            return 2;
        }
    }

    if(op==QUERY)
    {
        value = get_brightness(dev);
        printf("%.0f\n", value);
    }
    else
    {
        if(!set_brightness(dev, op, value))
        {
            fprintf(stderr, "Could not set brightness!\n");
            return 3;
        }
    }

    return 0;
}

