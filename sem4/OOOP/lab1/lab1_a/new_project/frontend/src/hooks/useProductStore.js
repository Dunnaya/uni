import { useState, useEffect } from 'react';
import { productStore } from '../store/productStore';

export const useProductStore = () => {
    const [products, setProducts] = useState(productStore.getProducts());

    useEffect(() => {
        const unsubscribe = productStore.subscribe(setProducts);
        return unsubscribe;
    }, []);

    return {
        products,
        addProduct: (product) => productStore.addProduct(product),
        updateProduct: (product) => productStore.updateProduct(product),
        removeProduct: (id) => productStore.removeProduct(id),
        setProducts: (products) => productStore.setProducts(products)
    };
};