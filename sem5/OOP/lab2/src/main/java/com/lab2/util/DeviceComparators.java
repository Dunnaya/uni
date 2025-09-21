package com.lab2.util;

import com.lab2.model.Device;
import java.util.Comparator;

public class DeviceComparators {
    // by name
    public static final Comparator<Device> BY_NAME = 
        Comparator.comparing(Device::getName, String.CASE_INSENSITIVE_ORDER);

    // by price (ascending)
    public static final Comparator<Device> BY_PRICE_ASC = 
        Comparator.comparing(Device::getPrice);

    // by price (descending)
    public static final Comparator<Device> BY_PRICE_DESC = 
        BY_PRICE_ASC.reversed();

    // by origin
    public static final Comparator<Device> BY_ORIGIN = 
        Comparator.comparing(Device::getOrigin, String.CASE_INSENSITIVE_ORDER);

    // by power consumption (ascending)
    public static final Comparator<Device> BY_POWER_CONSUMPTION = 
        Comparator.comparing(device -> device.getType().getPowerConsumption());

    // by criticality (critical first)
    public static final Comparator<Device> BY_CRITICAL = 
        Comparator.comparing(Device::isCritical).reversed();

    // by criticality (critical first, then by price ascending)
    public static final Comparator<Device> BY_CRITICAL_THEN_PRICE = 
        BY_CRITICAL.thenComparing(BY_PRICE_ASC);

    // private constructor for utility class
    private DeviceComparators() {
        throw new AssertionError("Utility class cannot be instantiated");
    }
}