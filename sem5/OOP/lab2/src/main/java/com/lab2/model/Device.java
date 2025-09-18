package com.lab2.model;

import java.util.List;
import java.util.Objects;

public class Device {
    private String id;
    private String name;
    private String origin;
    private double price;
    private ComponentType type;
    private boolean critical;

    // constructors
    public Device() {}

    public Device(String id, String name, String origin, double price, 
                 ComponentType type, boolean critical) {
        this.id = id;
        this.name = name;
        this.origin = origin;
        this.price = price;
        this.type = type;
        this.critical = critical;
    }

    // getters and setters
    public String getId() { return id; }
    public void setId(String id) { this.id = id; }

    public String getName() { return name; }
    public void setName(String name) { this.name = name; }

    public String getOrigin() { return origin; }
    public void setOrigin(String origin) { this.origin = origin; }

    public double getPrice() { return price; }
    public void setPrice(double price) { this.price = price; }

    public ComponentType getType() { return type; }
    public void setType(ComponentType type) { this.type = type; }

    public boolean isCritical() { return critical; }
    public void setCritical(boolean critical) { this.critical = critical; }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Device device = (Device) o;
        return Objects.equals(id, device.id);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id);
    }

    @Override
    public String toString() {
        return "Device{" +
                "id='" + id + '\'' +
                ", name='" + name + '\'' +
                ", origin='" + origin + '\'' +
                ", price=" + price +
                ", type=" + type +
                ", critical=" + critical +
                '}';
    }

    // inner class for component characteristics
    public static class ComponentType {
        private boolean peripheral;
        private int powerConsumption;
        private boolean coolerPresent;
        private ComponentGroup componentGroup;
        private List<Port> ports;

        public ComponentType() {}

        public ComponentType(boolean peripheral, int powerConsumption, 
                           boolean coolerPresent, ComponentGroup componentGroup, 
                           List<Port> ports) {
            this.peripheral = peripheral;
            this.powerConsumption = powerConsumption;
            this.coolerPresent = coolerPresent;
            this.componentGroup = componentGroup;
            this.ports = ports;
        }

        // getters and setters
        public boolean isPeripheral() { return peripheral; }
        public void setPeripheral(boolean peripheral) { this.peripheral = peripheral; }

        public int getPowerConsumption() { return powerConsumption; }
        public void setPowerConsumption(int powerConsumption) { 
            this.powerConsumption = powerConsumption; 
        }

        public boolean isCoolerPresent() { return coolerPresent; }
        public void setCoolerPresent(boolean coolerPresent) { 
            this.coolerPresent = coolerPresent; 
        }

        public ComponentGroup getComponentGroup() { return componentGroup; }
        public void setComponentGroup(ComponentGroup componentGroup) { 
            this.componentGroup = componentGroup; 
        }

        public List<Port> getPorts() { return ports; }
        public void setPorts(List<Port> ports) { this.ports = ports; }

        @Override
        public String toString() {
            return "ComponentType{" +
                    "peripheral=" + peripheral +
                    ", powerConsumption=" + powerConsumption +
                    ", coolerPresent=" + coolerPresent +
                    ", componentGroup=" + componentGroup +
                    ", ports=" + ports +
                    '}';
        }
    }

    // enum for component groups
    public enum ComponentGroup {
        INPUT_OUTPUT("input-output"),
        MULTIMEDIA("multimedia"),
        PROCESSING("processing"),
        MEMORY("memory"),
        STORAGE("storage");

        private final String value;

        ComponentGroup(String value) {
            this.value = value;
        }

        public String getValue() {
            return value;
        }

        public static ComponentGroup fromValue(String value) {
            for (ComponentGroup group : values()) {
                if (group.value.equals(value)) {
                    return group;
                }
            }
            throw new IllegalArgumentException("Unknown component group: " + value);
        }
    }

    // enum for ports
    public enum Port {
        COM, USB, LPT, HDMI, LGA1700, DDR5, DDR4;

        public static Port fromValue(String value) {
            try {
                return Port.valueOf(value.toUpperCase());
            } catch (IllegalArgumentException e) {
                throw new IllegalArgumentException("Unknown port: " + value);
            }
        }
    }
}