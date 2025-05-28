export const validateProduct = (data) => {
    const errors = [];

    if (!data.name || data.name.trim().length < 2)
        errors.push('Product name must be at least 2 characters long');

    if (!data.price || isNaN(data.price) || data.price <= 0)
        errors.push('Product price must be a positive number');

    if (!data.image || !data.image.trim())
        errors.push('Product image URL is required');

    return {
        isValid: errors.length === 0,
        errors,
        sanitizedData: {
            name: data.name?.trim(),
            price: parseFloat(data.price),
            image: data.image?.trim(),
            description: data.description?.trim() || ''
        }
    };
};

export const validateUser = (data) => {
    const errors = [];

    if (!data.name || data.name.trim().length < 2)
        errors.push('Name must be at least 2 characters long');

    if (!data.email || !isValidEmail(data.email))
        errors.push('Valid email is required');

    if (data.password !== undefined && data.password.length < 6)
        errors.push('Password must be at least 6 characters long');

    return {
        isValid: errors.length === 0,
        errors,
        sanitizedData: {
            name: data.name?.trim(),
            email: data.email?.toLowerCase().trim(),
            password: data.password,
            role: data.role || 'user'
        }
    };
};

const isValidEmail = (email) => {
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return emailRegex.test(email);
};