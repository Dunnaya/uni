export class DatabaseAdapter {
  async find(model, filter = {}) {
    throw new Error('Find method must be implemented');
  }

  async findById(model, id) {
    throw new Error('FindById method must be implemented');
  }

  async create(model, data) {
    throw new Error('Create method must be implemented');
  }

  async update(model, id, data) {
    throw new Error('Update method must be implemented');
  }

  async delete(model, id) {
    throw new Error('Delete method must be implemented');
  }
}

export class MongooseAdapter extends DatabaseAdapter {
  constructor(models) {
    super();
    this.models = models;
  }

  async find(modelName, filter = {}) {
    const Model = this.models[modelName];
    return await Model.find(filter);
  }

  async findById(modelName, id) {
    const Model = this.models[modelName];
    return await Model.findById(id);
  }

  async create(modelName, data) {
    const Model = this.models[modelName];
    const instance = new Model(data);
    return await instance.save();
  }

  async update(modelName, id, data) {
    const Model = this.models[modelName];
    return await Model.findByIdAndUpdate(id, data, { new: true });
  }

  async delete(modelName, id) {
    const Model = this.models[modelName];
    return await Model.findByIdAndDelete(id);
  }
}