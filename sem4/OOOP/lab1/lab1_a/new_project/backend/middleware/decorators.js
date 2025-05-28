import { ResponseDirector } from '../utils/responseBuilder.js';

/**
 * Base middleware decorator
 * @class MiddlewareDecorator
 */
export class MiddlewareDecorator {
  constructor(middleware) {
    this.middleware = middleware;
  }

  async execute(req, res, next) {
    return await this.middleware(req, res, next);
  }
}

/**
 * Logging decorator - logs request details
 */
export class LoggingDecorator extends MiddlewareDecorator {
  constructor(middleware, logger = console) {
    super(middleware);
    this.logger = logger;
  }

  async execute(req, res, next) {
    const start = Date.now();
    const { method, url, ip } = req;
    
    this.logger.log(`[${new Date().toISOString()}] ${method} ${url} - IP: ${ip}`);
    
    await this.middleware(req, res, next);
    
    const duration = Date.now() - start;
    this.logger.log(`[${new Date().toISOString()}] ${method} ${url} - Completed in ${duration}ms`);
  }
}

/**
 * Rate limiting decorator
 */
export class RateLimitDecorator extends MiddlewareDecorator {
  constructor(middleware, options = {}) {
    super(middleware);
    this.windowMs = options.windowMs || 15 * 60 * 1000; // 15 minutes
    this.maxRequests = options.maxRequests || 100;
    this.requests = new Map();
    this.responseDirector = new ResponseDirector();
  }

  async execute(req, res, next) {
    const clientId = req.ip || req.connection.remoteAddress;
    const now = Date.now();
    const windowStart = now - this.windowMs;

    this.cleanOldEntries(windowStart);

    if (!this.requests.has(clientId)) {
      this.requests.set(clientId, []);
    }

    const clientRequests = this.requests.get(clientId);

    const requestsInWindow = clientRequests.filter(time => time > windowStart);

    if (requestsInWindow.length >= this.maxRequests) {
      const response = this.responseDirector.buildErrorResponse(
        'Too many requests, please try again later',
        'Rate limit exceeded'
      );
      return res.status(429).json(response);
    }

    // Add current request
    requestsInWindow.push(now);
    this.requests.set(clientId, requestsInWindow);

    // Execute original middleware
    await this.middleware(req, res, next);
  }

  cleanOldEntries(windowStart) {
    for (const [clientId, requests] of this.requests.entries()) {
      const recentRequests = requests.filter(time => time > windowStart);
      if (recentRequests.length === 0) {
        this.requests.delete(clientId);
      } else {
        this.requests.set(clientId, recentRequests);
      }
    }
  }
}

/**
 * Validation decorator - adds validation to routes
 */
export class ValidationDecorator extends MiddlewareDecorator {
  constructor(middleware, validationStrategy) {
    super(middleware);
    this.validationStrategy = validationStrategy;
    this.responseDirector = new ResponseDirector();
  }

  async execute(req, res, next) {
    const validation = this.validationStrategy.validate(req.body);
    
    if (!validation.isValid) {
      const response = this.responseDirector.buildValidationErrorResponse(validation.errors);
      return res.status(400).json(response);
    }

    req.body = validation.sanitizedData;

    await this.middleware(req, res, next);
  }
}

/**
 * Error handling decorator
 */
export class ErrorHandlingDecorator extends MiddlewareDecorator {
  constructor(middleware, logger = console) {
    super(middleware);
    this.logger = logger;
    this.responseDirector = new ResponseDirector();
  }

  async execute(req, res, next) {
    try {
      await this.middleware(req, res, next);
    } catch (error) {
      this.logger.error('Middleware error:', error);
      
      let statusCode = 500;
      let message = 'Internal server error';

      if (error.name === 'ValidationError') {
        statusCode = 400;
        message = 'Validation error';
      } else if (error.name === 'CastError') {
        statusCode = 400;
        message = 'Invalid ID format';
      } else if (error.code === 11000) {
        statusCode = 400;
        message = 'Duplicate field value';
      }

      const response = this.responseDirector.buildErrorResponse(
        error.message,
        message
      );

      res.status(statusCode).json(response);
    }
  }
}

// decorator factory functions
export const withLogging = (middleware) => new LoggingDecorator(middleware);
export const withRateLimit = (middleware, options) => new RateLimitDecorator(middleware, options);
export const withValidation = (middleware, strategy) => new ValidationDecorator(middleware, strategy);
export const withErrorHandling = (middleware) => new ErrorHandlingDecorator(middleware);