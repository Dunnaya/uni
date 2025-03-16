import { useEffect, useState } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import {
  Container,
  Box,
  Image,
  Text,
  Heading,
  VStack,
  HStack,
  Button,
  useColorModeValue,
  Spinner,
  Divider,
  useToast,
  Modal,
  ModalOverlay,
  ModalContent,
  ModalHeader,
  ModalFooter,
  ModalBody,
  ModalCloseButton,
  Input,
  Textarea,
  useDisclosure,
  NumberInput,
  NumberInputField,
  NumberInputStepper,
  NumberIncrementStepper,
  NumberDecrementStepper
} from '@chakra-ui/react';
import { ArrowBackIcon, EditIcon, DeleteIcon, AddIcon } from '@chakra-ui/icons';
import { useProductStore } from '../store/product';
import { useAuthStore } from '../store/auth';
import { useCartStore } from '../store/cart';

const ProductPage = () => {
  const { productId } = useParams();
  const navigate = useNavigate();
  const toast = useToast();
  const { isAuthenticated, isAdmin } = useAuthStore();
  const { products, deleteProduct, updateProduct } = useProductStore();
  const { addToCart } = useCartStore();
  const { isOpen, onOpen, onClose } = useDisclosure();
  
  const [product, setProduct] = useState(null);
  const [updatedProduct, setUpdatedProduct] = useState(null);
  const [loading, setLoading] = useState(true);
  const [quantity, setQuantity] = useState(1);

  const bgColor = useColorModeValue('white', 'rgb(40, 4, 30)');
  const textColor = useColorModeValue('pink.600', 'gray.400');

  useEffect(() => {
    const fetchProduct = async () => {
      // First check if we already have the product in our store
      if (products && products.length > 0) {
        const foundProduct = products.find(p => p._id === productId);
        if (foundProduct) {
          setProduct(foundProduct);
          setUpdatedProduct(foundProduct);
          setLoading(false);
          return;
        }
      }

      // If not found in store, fetch it from API
      try {
        const response = await fetch(`/api/products/${productId}`);
        const data = await response.json();
        
        if (data.success) {
          setProduct(data.data);
          setUpdatedProduct(data.data);
        } else {
          toast({
            title: 'Error',
            description: 'Product not found',
            status: 'error',
            duration: 3000,
            isClosable: true,
          });
          navigate('/');
        }
      } catch (error) {
        toast({
          title: 'Error',
          description: 'Failed to load product',
          status: 'error',
          duration: 3000,
          isClosable: true,
        });
      } finally {
        setLoading(false);
      }
    };

    fetchProduct();
  }, [productId, products, navigate, toast]);

  const handleDelete = async () => {
    const { success, message } = await deleteProduct(productId);
    
    if (success) {
      toast({
        title: 'Success',
        description: 'Product deleted successfully',
        status: 'success',
        duration: 3000,
        isClosable: true,
      });
      navigate('/');
      return;
    } 
    
    toast({
      title: 'Error',
      description: message,
      status: 'error',
      duration: 3000,
      isClosable: true,
    });
  };
  
  const handleUpdateProduct = async () => {
    const { success, message } = await updateProduct(productId, updatedProduct);
    if (success) {
      toast({
        title: 'Success',
        description: 'Product updated successfully',
        status: 'success',
        duration: 3000,
        isClosable: true,
      });
      setProduct(updatedProduct);
      onClose();
    } else {
      toast({
        title: 'Error',
        description: message,
        status: 'error',
        duration: 3000,
        isClosable: true,
      });
    }
  };

  const handleAddToCart = () => {
    addToCart(product, quantity);
    toast({
      title: 'Added to Cart',
      description: `${quantity} x ${product.name} added to your cart`,
      status: 'success',
      duration: 3000,
      isClosable: true,
    });
    setQuantity(1);
  };

  if (loading) {
    return (
      <Container centerContent py={10}>
        <Spinner size="xl" color="pink.500" />
      </Container>
    );
  }

  if (!product) {
    return (
      <Container centerContent py={10}>
        <Text>Product not found</Text>
        <Button leftIcon={<ArrowBackIcon />} onClick={() => navigate('/')} mt={4}>
          Back to Products
        </Button>
      </Container>
    );
  }

  return (
    <Container maxW="container.xl" py={10}>
      <Button 
        leftIcon={<ArrowBackIcon />} 
        onClick={() => navigate('/')} 
        mb={6}
        variant="outline"
      >
        Back to Products
      </Button>

      <Box
        bg={bgColor}
        shadow="xl"
        rounded="lg"
        overflow="hidden"
        w="full"
      >
        <Box display={{ md: 'flex' }} alignItems="center">
          <Box w={{ base: 'full', md: '50%' }} h={{ base: '300px', md: '500px' }}>
            <Image
              src={product.image}
              alt={product.name}
              w="full"
              h="full"
              objectFit="cover"
            />
          </Box>

          <VStack 
            alignItems="flex-start" 
            p={8} 
            w={{ base: 'full', md: '50%' }} 
            spacing={6}
          >
            <Heading as="h1" size="xl">
              {product.name}
            </Heading>
            
            <Text fontWeight="bold" fontSize="2xl" color={textColor}>
              ${product.price}
            </Text>
            
            <Divider />
            
            <Box>
              <Heading as="h3" size="md" mb={2}>
                Description
              </Heading>
              <Text>{product.description || 'No description available'}</Text>
            </Box>

            {isAuthenticated && (
              <Box w="full" pt={4}>
                <HStack spacing={4} mb={4}>
                  <Text>Quantity:</Text>
                  <NumberInput 
                    min={1} 
                    max={100} 
                    value={quantity} 
                    onChange={(value) => setQuantity(parseInt(value))}
                    w="100px"
                  >
                    <NumberInputField />
                    <NumberInputStepper>
                      <NumberIncrementStepper />
                      <NumberDecrementStepper />
                    </NumberInputStepper>
                  </NumberInput>
                </HStack>
                <Button 
                  colorScheme="purple" 
                  w="full" 
                  leftIcon={<AddIcon />}
                  onClick={handleAddToCart}
                >
                  Add to Cart
                </Button>
              </Box>
            )}

            {isAdmin && (
              <HStack spacing={4} pt={4} w="full">
                <Button 
                  colorScheme="green" 
                  w="full"
                  leftIcon={<EditIcon />}
                  onClick={onOpen}
                >
                  Edit Product
                </Button>
                <Button 
                  colorScheme="pink" 
                  w="full"
                  leftIcon={<DeleteIcon />}
                  onClick={handleDelete}
                >
                  Delete Product
                </Button>
              </HStack>
            )}
          </VStack>
        </Box>
      </Box>
      
      {/* Edit Product Modal */}
      <Modal isOpen={isOpen} onClose={onClose}>
        <ModalOverlay />
        <ModalContent>
          <ModalHeader>Edit Product</ModalHeader>
          <ModalCloseButton />
          <ModalBody>
            <VStack spacing={4}>
              <Input
                placeholder='Product Name'
                name='name'
                value={updatedProduct?.name || ''}
                onChange={(e) => setUpdatedProduct({ ...updatedProduct, name: e.target.value })}
              />
              <Input
                placeholder='Price'
                name='price'
                type='number'
                value={updatedProduct?.price || ''}
                onChange={(e) => setUpdatedProduct({ ...updatedProduct, price: e.target.value })}
              />
              <Input
                placeholder='Image URL'
                name='image'
                value={updatedProduct?.image || ''}
                onChange={(e) => setUpdatedProduct({ ...updatedProduct, image: e.target.value })}
              />
              <Textarea
                placeholder='Product Description'
                name='description'
                value={updatedProduct?.description || ''}
                onChange={(e) => setUpdatedProduct({ ...updatedProduct, description: e.target.value })}
                rows={4}
              />
            </VStack>
          </ModalBody>

          <ModalFooter>
            <Button
              colorScheme='pink'
              mr={3}
              onClick={handleUpdateProduct}
            >
              Update
            </Button>
            <Button variant='ghost' onClick={onClose}>
              Cancel
            </Button>
          </ModalFooter>
        </ModalContent>
      </Modal>
    </Container>
  );
};

export default ProductPage;