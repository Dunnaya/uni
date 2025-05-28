export class DIContainer {
  constructor() {
    this.services = new Map();
    this.singletons = new Map();
  }

  register(name, factory, singleton = false) {
    this.services.set(name, { factory, singleton });
    return this;
  }

  get(name) {
    const service = this.services.get(name);
    if (!service) {
      throw new Error(`Service ${name} not found`);
    }

    if (service.singleton) {
      if (!this.singletons.has(name)) {
        this.singletons.set(name, service.factory(this));
      }
      return this.singletons.get(name);
    }

    return service.factory(this);
  }
}

export function setupDIContainer() {
  const container = new DIContainer();

  container.register('models', () => ({
    Product: Product,
    User: User
  }), true);

  container.register('databaseAdapter', (c) => 
    new MongooseAdapter(c.get('models')), true);

  container.register('productValidationStrategy', () => 
    new ProductValidationStrategy());
  
  container.register('userValidationStrategy', () => 
    new UserValidationStrategy());

  container.register('responseDirector', () => 
    new ResponseDirector(), true);

  container.register('productService', (c) => 
    new ProductService(
      c.get('databaseAdapter'),
      c.get('productValidationStrategy'),
      c.get('responseDirector')
    ));

  return container;
}