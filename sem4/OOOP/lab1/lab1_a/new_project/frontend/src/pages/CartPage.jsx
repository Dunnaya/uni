import { useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
import {
  Box,
  Button,
  Container,
  Divider,
  Flex,
  Heading,
  HStack,
  IconButton,
  Image,
  NumberInput,
  NumberInputField,
  NumberInputStepper,
  NumberIncrementStepper,
  NumberDecrementStepper,
  Text,
  VStack,
  useColorModeValue,
  useToast
} from '@chakra-ui/react';
import { DeleteIcon, ArrowBackIcon } from '@chakra-ui/icons';
import { useCartStore } from '../store/cart';
import { useAuthStore } from '../store/auth';

/**
 * @file Shopping cart page component
 * @module pages/CartPage
 */

/**
 * CartPage component displays the user's shopping cart with product items,
 * quantities, prices, and totals. Allows users to update quantities, remove items,
 * and proceed to checkout.
 * 
 * @component
 * @returns {JSX.Element} - Rendered CartPage component
 */

const CartPage = () => {
  const { items, totalItems, totalPrice, removeFromCart, updateQuantity, clearCart } = useCartStore();
  const { isAuthenticated } = useAuthStore();
  const navigate = useNavigate();
  const toast = useToast();
  
  // redirect if not authenticated
  useEffect(() => {
    if (!isAuthenticated) {
      toast({
        title: 'Authentication Required',
        description: 'Please log in to view your cart',
        status: 'warning',
        duration: 3000,
        isClosable: true,
      });
      navigate('/login');
    }
  }, [isAuthenticated, navigate, toast]);

  /**
 * Handles the checkout process
 * Clears the cart after checkout
 * 
 * @function
 * @returns {void}
 */

  const handleCheckout = () => {
    toast({
      title: 'Checkout Process',
      description: 'Checkout functionality can be implemented here',
      status: 'info',
      duration: 3000,
      isClosable: true,
    });
    clearCart();
  };

  const bgColor = useColorModeValue('white', 'rgb(40, 4, 30)');
  const borderColor = useColorModeValue('gray.200', 'gray.700');

  if (!isAuthenticated) {
    return null; // don't render anything while redirecting
  }

  return (
    <Container maxW="container.xl" py={10}>
      <Button 
        leftIcon={<ArrowBackIcon />} 
        onClick={() => navigate('/')} 
        mb={6}
        variant="outline"
      >
        Continue Shopping
      </Button>

      <Heading 
        as="h1" 
        size="xl" 
        mb={8}
        textAlign="center"
        bgGradient="linear(to-r, rgb(233, 25, 132), rgb(152, 16, 186))"
        bgClip="text"
      >
        Your Shopping Cart
      </Heading>

      {items.length === 0 ? (
        <Box textAlign="center" py={10}>
          <Heading as="h3" size="md" mb={4}>
            Your cart is empty
          </Heading>
          <Button 
            colorScheme="pink" 
            onClick={() => navigate('/')}
          >
            Shop Now
          </Button>
        </Box>
      ) : (
        <Flex 
          direction={{ base: 'column', lg: 'row' }} 
          align="flex-start" 
          justify="space-between"
          gap={8}
        >
          <VStack spacing={4} flex="1" align="stretch" w="full">
            {items.map((item) => (
              <Box 
                key={item._id}
                p={4}
                borderWidth="1px"
                borderRadius="lg"
                borderColor={borderColor}
                bg={bgColor}
                shadow="md"
              >
                <Flex gap={4} align="center">
                  <Image 
                    src={item.image} 
                    alt={item.name} 
                    boxSize="100px" 
                    objectFit="cover"
                    borderRadius="md"
                  />
                  <VStack align="flex-start" flex="1" spacing={1}>
                    <Text fontWeight="bold" fontSize="lg">{item.name}</Text>
                    <Text color="gray.500" fontSize="sm" noOfLines={2}>
                      {item.description || 'No description available'}
                    </Text>
                    <Text fontWeight="medium" color="pink.500">
                      ${item.price}
                    </Text>
                  </VStack>
                  <VStack>
                    <HStack>
                      <Text>Qty:</Text>
                      <NumberInput 
                        min={1} 
                        max={100} 
                        value={item.quantity}
                        onChange={(value) => updateQuantity(item._id, parseInt(value))}
                        size="sm"
                        w="80px"
                      >
                        <NumberInputField />
                        <NumberInputStepper>
                          <NumberIncrementStepper />
                          <NumberDecrementStepper />
                        </NumberInputStepper>
                      </NumberInput>
                    </HStack>
                    <Text fontWeight="bold">
                      ${(item.price * item.quantity).toFixed(2)}
                    </Text>
                  </VStack>
                  <IconButton
                    icon={<DeleteIcon />}
                    colorScheme="red"
                    variant="ghost"
                    onClick={() => removeFromCart(item._id)}
                    aria-label="Remove item"
                  />
                </Flex>
              </Box>
            ))}
          </VStack>

          <Box 
            w={{ base: 'full', lg: '350px' }}
            p={6}
            borderWidth="1px"
            borderRadius="lg"
            borderColor={borderColor}
            bg={bgColor}
            shadow="md"
            position="sticky"
            top="100px"
          >
            <Heading as="h3" size="md" mb={4}>
              Order Summary
            </Heading>
            
            <VStack spacing={3} align="stretch">
              <Flex justify="space-between">
                <Text>Items ({totalItems}):</Text>
                <Text>${totalPrice.toFixed(2)}</Text>
              </Flex>
              
              <Flex justify="space-between">
                <Text>Shipping:</Text>
                <Text>Free</Text>
              </Flex>
              
              <Divider my={2} />
              
              <Flex justify="space-between" fontWeight="bold">
                <Text>Total:</Text>
                <Text>${totalPrice.toFixed(2)}</Text>
              </Flex>
              
              <Button 
                colorScheme="pink" 
                size="lg" 
                w="full" 
                mt={4}
                onClick={handleCheckout}
              >
                Checkout
              </Button>
              
              <Button 
                variant="outline"
                onClick={clearCart}
                size="sm"
                mt={2}
              >
                Clear Cart
              </Button>
            </VStack>
          </Box>
        </Flex>
      )}
    </Container>
  );
};

export default CartPage;