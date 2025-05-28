/**
 * Builder pattern for constructing API responses
 * @class ResponseBuilder
 */
export class ResponseBuilder {
  constructor() {
    this.reset();
  }

  reset() {
    this.response = {
      success: true,
      message: null,
      data: null,
      meta: {},
      errors: []
    };
    return this;
  }

  success(isSuccess = true) {
    this.response.success = isSuccess;
    return this;
  }

  message(msg) {
    this.response.message = msg;
    return this;
  }

  data(data) {
    this.response.data = data;
    return this;
  }

  meta(key, value) {
    if (typeof key === 'object') {
      this.response.meta = { ...this.response.meta, ...key };
    } else {
      this.response.meta[key] = value;
    }
    return this;
  }

  pagination(page, limit, total, totalPages) {
    this.response.meta.pagination = {
      page,
      limit,
      total,
      totalPages,
      hasNext: page < totalPages,
      hasPrev: page > 1
    };
    return this;
  }

  error(error) {
    this.response.success = false;
    if (typeof error === 'string') {
      this.response.errors.push(error);
    } else if (Array.isArray(error)) {
      this.response.errors = [...this.response.errors, ...error];
    }
    return this;
  }

  build() {
    const result = { ...this.response };
    
    if (result.errors.length === 0) delete result.errors;
    if (Object.keys(result.meta).length === 0) delete result.meta;
    if (result.data === null) delete result.data;
    
    return result;
  }
}

export class ResponseDirector {
  constructor() {
    this.builder = new ResponseBuilder();
  }

  buildSuccessResponse(data, message = null) {
    return this.builder
      .reset()
      .success(true)
      .data(data)
      .message(message)
      .build();
  }

  buildErrorResponse(errors, message = 'Operation failed') {
    return this.builder
      .reset()
      .success(false)
      .message(message)
      .error(errors)
      .build();
  }

  buildPaginatedResponse(data, page, limit, total) {
    const totalPages = Math.ceil(total / limit);
    return this.builder
      .reset()
      .success(true)
      .data(data)
      .pagination(page, limit, total, totalPages)
      .build();
  }

  buildValidationErrorResponse(validationErrors) {
    return this.builder
      .reset()
      .success(false)
      .message('Validation failed')
      .error(validationErrors)
      .build();
  }
}