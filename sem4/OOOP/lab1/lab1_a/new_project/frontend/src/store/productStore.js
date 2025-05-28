class SimpleProductStore 
{
    static instance;

    constructor() 
    {
        if (SimpleProductStore.instance) {
            return SimpleProductStore.instance;
        }

        this.listeners = [];
        this.products = [];

        SimpleProductStore.instance = this;
    }

    subscribe(listener) 
    {
        this.listeners.push(listener);
        return () => {
            this.listeners = this.listeners.filter(l => l !== listener);
        };
    }

    notify() 
    {
        this.listeners.forEach(listener => listener(this.products));
    }

    setProducts(products) 
    {
        this.products = products;
        this.notify();
    }

    addProduct(product) 
    {
        this.products.push(product);
        this.notify();
    }

    updateProduct(updatedProduct) 
    {
        const index = this.products.findIndex(p => p._id === updatedProduct._id);
        if (index !== -1) {
            this.products[index] = updatedProduct;
            this.notify();
        }
    }

    removeProduct(productId) 
    {
        this.products = this.products.filter(p => p._id !== productId);
        this.notify();
    }

    getProducts() 
    {
        return this.products;
    }
}

export const productStore = new SimpleProductStore();