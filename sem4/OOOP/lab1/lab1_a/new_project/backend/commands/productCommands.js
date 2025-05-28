import { ResponseDirector } from '../utils/responseBuilder.js';

/**
 * Base command class
 * @class Command
 */
export class Command {
  async execute() {
    throw new Error('Execute method must be implemented');
  }

  async undo() {
    throw new Error('Undo method must be implemented');
  }
}

/**
 * Create product command
 */
export class CreateProductCommand extends Command {
  constructor(databaseAdapter, productData, userId) {
    super();
    this.db = databaseAdapter;
    this.productData = productData;
    this.userId = userId;
    this.createdProduct = null;
    this.responseDirector = new ResponseDirector();
  }

  async execute() {
    try {
      const productToCreate = {
        ...this.productData,
        createdBy: this.userId,
        createdAt: new Date()
      };

      this.createdProduct = await this.db.create('Product', productToCreate);
      
      return this.responseDirector.buildSuccessResponse(
        this.createdProduct,
        'Product created successfully'
      );
    } catch (error) {
      return this.responseDirector.buildErrorResponse(
        error.message,
        'Failed to create product'
      );
    }
  }

  async undo() {
    if (this.createdProduct) {
      try {
        await this.db.delete('Product', this.createdProduct._id);
        return this.responseDirector.buildSuccessResponse(
          null,
          'Product creation undone'
        );
      } catch (error) {
        return this.responseDirector.buildErrorResponse(
          error.message,
          'Failed to undo product creation'
        );
      }
    }
    return this.responseDirector.buildErrorResponse(
      'No product to undo',
      'Undo operation failed'
    );
  }
}

/**
 * Update product command
 */
export class UpdateProductCommand extends Command {
  constructor(databaseAdapter, productId, updateData, userId) {
    super();
    this.db = databaseAdapter;
    this.productId = productId;
    this.updateData = updateData;
    this.userId = userId;
    this.originalProduct = null;
    this.updatedProduct = null;
    this.responseDirector = new ResponseDirector();
  }

  async execute() {
    try {
      this.originalProduct = await this.db.findById('Product', this.productId);
      
      if (!this.originalProduct) {
        return this.responseDirector.buildErrorResponse(
          'Product not found',
          'Update failed'
        );
      }

      const dataToUpdate = {
        ...this.updateData,
        updatedBy: this.userId,
        updatedAt: new Date()
      };

      this.updatedProduct = await this.db.update('Product', this.productId, dataToUpdate);
      
      return this.responseDirector.buildSuccessResponse(
        this.updatedProduct,
        'Product updated successfully'
      );
    } catch (error) {
      return this.responseDirector.buildErrorResponse(
        error.message,
        'Failed to update product'
      );
    }
  }

  async undo() {
    if (this.originalProduct && this.updatedProduct) {
      try {
        const restoredProduct = await this.db.update('Product', this.productId, {
          name: this.originalProduct.name,
          price: this.originalProduct.price,
          image: this.originalProduct.image,
          description: this.originalProduct.description
        });

        return this.responseDirector.buildSuccessResponse(
          restoredProduct,
          'Product update undone'
        );
      } catch (error) {
        return this.responseDirector.buildErrorResponse(
          error.message,
          'Failed to undo product update'
        );
      }
    }
    return this.responseDirector.buildErrorResponse(
      'No product update to undo',
      'Undo operation failed'
    );
  }
}

/**
 * Delete product command
 */
export class DeleteProductCommand extends Command {
  constructor(databaseAdapter, productId, userId) {
    super();
    this.db = databaseAdapter;
    this.productId = productId;
    this.userId = userId;
    this.deletedProduct = null;
    this.responseDirector = new ResponseDirector();
  }

  async execute() {
    try {
      this.deletedProduct = await this.db.findById('Product', this.productId);
      
      if (!this.deletedProduct) {
        return this.responseDirector.buildErrorResponse(
          'Product not found',
          'Delete failed'
        );
      }

      await this.db.delete('Product', this.productId);
      
      return this.responseDirector.buildSuccessResponse(
        { id: this.productId },
        'Product deleted successfully'
      );
    } catch (error) {
      return this.responseDirector.buildErrorResponse(
        error.message,
        'Failed to delete product'
      );
    }
  }

  async undo() {
    if (this.deletedProduct) {
      try {
        // recreate the product without the _id to let mongoDB generate a new one
        const { _id, ...productData } = this.deletedProduct.toObject ? 
          this.deletedProduct.toObject() : this.deletedProduct;

        const restoredProduct = await this.db.create('Product', productData);
        
        return this.responseDirector.buildSuccessResponse(
          restoredProduct,
          'Product deletion undone'
        );
      } catch (error) {
        return this.responseDirector.buildErrorResponse(
          error.message,
          'Failed to undo product deletion'
        );
      }
    }
    return this.responseDirector.buildErrorResponse(
      'No product deletion to undo',
      'Undo operation failed'
    );
  }
}

/**
 * Batch update products command
 */
export class BatchUpdateProductsCommand extends Command {
  constructor(databaseAdapter, updates, userId) {
    super();
    this.db = databaseAdapter;
    this.updates = updates; // arr of { id, data }
    this.userId = userId;
    this.originalProducts = [];
    this.updatedProducts = [];
    this.responseDirector = new ResponseDirector();
  }

  async execute() {
    try {
      const results = [];
      
      for (const update of this.updates) {
        const original = await this.db.findById('Product', update.id);
        if (original) {
          this.originalProducts.push(original);

          const dataToUpdate = {
            ...update.data,
            updatedBy: this.userId,
            updatedAt: new Date()
          };

          const updated = await this.db.update('Product', update.id, dataToUpdate);
          this.updatedProducts.push(updated);
          results.push(updated);
        }
      }

      return this.responseDirector.buildSuccessResponse(
        results,
        `${results.length} products updated successfully`
      );
    } catch (error) {
      return this.responseDirector.buildErrorResponse(
        error.message,
        'Failed to batch update products'
      );
    }
  }

  async undo() {
    if (this.originalProducts.length > 0) {
      try {
        const results = [];
        
        for (const original of this.originalProducts) {
          const restored = await this.db.update('Product', original._id, {
            name: original.name,
            price: original.price,
            image: original.image,
            description: original.description
          });
          results.push(restored);
        }

        return this.responseDirector.buildSuccessResponse(
          results,
          `${results.length} product updates undone`
        );
      } catch (error) {
        return this.responseDirector.buildErrorResponse(
          error.message,
          'Failed to undo batch update'
        );
      }
    }
    return this.responseDirector.buildErrorResponse(
      'No batch update to undo',
      'Undo operation failed'
    );
  }
}

/**
 * Command invoker with history
 */
export class ProductCommandInvoker {
  constructor() {
    this.history = [];
    this.currentPosition = -1;
  }

  async execute(command) {
    this.history = this.history.slice(0, this.currentPosition + 1);
    
    const result = await command.execute();
    
    if (result.success) {
      this.history.push(command);
      this.currentPosition++;
    }
    
    return result;
  }

  async undo() {
    if (this.currentPosition >= 0) {
      const command = this.history[this.currentPosition];
      const result = await command.undo();
      
      if (result.success) {
        this.currentPosition--;
      }
      
      return result;
    }
    
    const responseDirector = new ResponseDirector();
    return responseDirector.buildErrorResponse(
      'Nothing to undo',
      'Undo operation failed'
    );
  }

  async redo() {
    if (this.currentPosition < this.history.length - 1) {
      this.currentPosition++;
      const command = this.history[this.currentPosition];
      return await command.execute();
    }
    
    const responseDirector = new ResponseDirector();
    return responseDirector.buildErrorResponse(
      'Nothing to redo',
      'Redo operation failed'
    );
  }

  canUndo() {
    return this.currentPosition >= 0;
  }

  canRedo() {
    return this.currentPosition < this.history.length - 1;
  }

  getHistory() {
    return this.history.map((command, index) => ({
      index,
      type: command.constructor.name,
      executed: index <= this.currentPosition
    }));
  }
}