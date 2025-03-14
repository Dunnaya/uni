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
  useToast
} from '@chakra-ui/react';
import { ArrowBackIcon } from '@chakra-ui/icons';
import { useProductStore } from '../store/product';
import { useAuthStore } from '../store/auth';

const ProductPage = () => {
  const { productId } = useParams();
  const navigate = useNavigate();
  const toast = useToast();
  const [product, setProduct] = useState(null);
  const [loading, setLoading] = useState(true);
  const { isAdmin } = useAuthStore();
  const { products, deleteProduct } = useProductStore();

  const bgColor = useColorModeValue('white', 'rgb(40, 4, 30)');
  const textColor = useColorModeValue('pink.600', 'gray.400');

  useEffect(() => {
    const fetchProduct = async () => {
      // first check if we already have the product in our store
      if (products && products.length > 0) {
        const foundProduct = products.find(p => p._id === productId);
        if (foundProduct) {
          setProduct(foundProduct);
          setLoading(false);
          return;
        }
      }

      // if not found in store, fetch it from API
      try {
        const response = await fetch(`/api/products/${productId}`);
        const data = await response.json();
        
        if (data.success) {
          setProduct(data.data);
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

            {isAdmin && (
              <HStack spacing={4} pt={4} w="full">
                <Button 
                  colorScheme="green" 
                  w="full"
                  onClick={() => navigate(`/edit/${product._id}`)}
                >
                  Edit Product
                </Button>
                <Button 
                  colorScheme="pink" 
                  w="full"
                  onClick={handleDelete}
                >
                  Delete Product
                </Button>
              </HStack>
            )}
          </VStack>
        </Box>
      </Box>
    </Container>
  );
};

export default ProductPage;