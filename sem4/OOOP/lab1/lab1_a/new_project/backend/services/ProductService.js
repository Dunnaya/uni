/**
 * Product service
 */
export class ProductService {
  constructor(databaseAdapter, validationStrategy, responseDirector) {
    this.db = databaseAdapter;
    this.validator = validationStrategy;
    this.responseDirector = responseDirector;
  }

  async getAllProducts() {
    try {
      const products = await this.db.find('Product');
      return this.responseDirector.buildSuccessResponse(products);
    } catch (error) {
      return this.responseDirector.buildErrorResponse(
        error.message,
        'Failed to fetch products'
      );
    }
  }

  async createProduct(productData, userId) {
    const validation = this.validator.validate(productData);
    
    if (!validation.isValid) {
      return this.responseDirector.buildValidationErrorResponse(validation.errors);
    }

    try {
      const productToCreate = {
        ...validation.sanitizedData,
        createdBy: userId
      };

      const product = await this.db.create('Product', productToCreate);
      return this.responseDirector.buildSuccessResponse(
        product,
        'Product created successfully'
      );
    } catch (error) {
      return this.responseDirector.buildErrorResponse(
        error.message,
        'Failed to create product'
      );
    }
  }

  async updateProduct(productId, updateData, userId) {
    try {
      const existingProduct = await this.db.findById('Product', productId);
      
      if (!existingProduct) {
        return this.responseDirector.buildErrorResponse(
          'Product not found',
          'Update failed'
        );
      }

      const dataToUpdate = {
        ...updateData,
        updatedBy: userId,
        updatedAt: new Date()
      };

      const updatedProduct = await this.db.update('Product', productId, dataToUpdate);
      
      return this.responseDirector.buildSuccessResponse(
        updatedProduct,
        'Product updated successfully'
      );
    } catch (error) {
      return this.responseDirector.buildErrorResponse(
        error.message,
        'Failed to update product'
      );
    }
  }

  async deleteProduct(productId) {
    try {
      const product = await this.db.findById('Product', productId);
      
      if (!product) {
        return this.responseDirector.buildErrorResponse(
          'Product not found',
          'Delete failed'
        );
      }

      await this.db.delete('Product', productId);
      
      return this.responseDirector.buildSuccessResponse(
        { id: productId },
        'Product deleted successfully'
      );
    } catch (error) {
      return this.responseDirector.buildErrorResponse(
        error.message,
        'Failed to delete product'
      );
    }
  }
}