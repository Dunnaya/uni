def count_operations_greville(m, n, iterations=None):
    operations = n + n  # init dot product and division

    for i in range(1, m):
        operations += n * n * i  # A_plus @ current_matrix
        operations += n * n  # subtraction from identity
        operations += n * n  # z @ a
        operations += n  # a.T @ (z @ a)
        operations += n * i * i  # A_plus @ A_plus.T
        operations += n * i  # r @ a
        operations += n  # a.T @ (r @ a)
        operations += 1  # addition
        operations += n * i  # a.T @ A_plus
        operations += n * n  # z @ (a @ (a.T @ A_plus))
        operations += n * i  # subtraction and division
        operations += n * i  # r @ a and division
        operations += n * (i + 1)  # hstack

    return operations


def count_operations_moore_penrose(m, n, iterations):
    operations = 0
    
    # init calculation
    operations += m * n * m  # A @ A.T
    operations += m * m  # Adding delta**2 * np.identity(m)
    operations += m * m * m  # Matrix inversion
    operations += n * m * m  # A.T @ inv(...)

    # per iteration
    operations += iterations * (
        m * n * m +  # A @ A.T
        m * m +  # Adding delta**2 * np.identity(m)
        m * m * m +  # Matrix inversion
        n * m * m +  # A.T @ inv(...)
        n * m +  # Matrix subtraction
        n * m  # Frobenius norm calculation
    )

    return operations