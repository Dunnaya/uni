import { DeleteIcon, EditIcon } from "@chakra-ui/icons";
import {
  Box,
  Button,
  Heading,
  HStack,
  IconButton,
  Image,
  Input,
  Modal,
  ModalBody,
  ModalCloseButton,
  ModalContent,
  ModalFooter,
  ModalHeader,
  ModalOverlay,
  Text,
  Textarea,
  useColorModeValue,
  useDisclosure,
  useToast,
  VStack,
} from "@chakra-ui/react";
import { useProductStore } from "../store/product";
import { useAuthStore } from "../store/auth";
import { useState } from "react";
import { Link } from "react-router-dom";

const ProductCard = ({ product }) => {
  const [updatedProduct, setUpdatedProduct] = useState(product);

  const textColor = useColorModeValue("pink.600", "gray.400");
  const bg = useColorModeValue("white", "rgb(40, 4, 30)");

  const { deleteProduct, updateProduct } = useProductStore();
  const { isAdmin } = useAuthStore();
  const toast = useToast();
  const { isOpen, onOpen, onClose } = useDisclosure();

  const handleDeleteProduct = async (pid) => {
    const { success, message } = await deleteProduct(pid);
    if (!success) {
      toast({
        title: "Error",
        description: message,
        status: "error",
        duration: 3000,
        isClosable: true,
      });
    } else {
      toast({
        title: "Success",
        description: message,
        status: "success",
        duration: 3000,
        isClosable: true,
      });
    }
  };

  const handleUpdateProduct = async (pid, updatedProduct) => {
    const { success, message } = await updateProduct(pid, updatedProduct);
    onClose();
    if (!success) {
      toast({
        title: "Error",
        description: message,
        status: "error",
        duration: 3000,
        isClosable: true,
      });
    } else {
      toast({
        title: "Success",
        description: "Product updated successfully",
        status: "success",
        duration: 3000,
        isClosable: true,
      });
    }
  };

  return (
    <Box
      shadow='lg'
      rounded='lg'
      overflow='hidden'
      transition='all 0.3s'
      _hover={{ transform: "translateY(-5px)", shadow: "xl" }}
      bg={bg}
    >
      <Link to={`/product/${product._id}`}>
        <Image src={product.image} alt={product.name} h={48} w='full' objectFit='cover' />

        <Box p={4}>
          <Heading as='h3' size='md' mb={2}>
            {product.name}
          </Heading>

          <Text fontWeight='bold' fontSize='xl' color={textColor} mb={4}>
            ${product.price}
          </Text>
        </Box>
      </Link>

      {isAdmin && (
        <Box p={4} pt={0}>
          <HStack spacing={2}>
            <IconButton icon={<EditIcon />} onClick={onOpen} colorScheme='green' />
            <IconButton
              icon={<DeleteIcon />}
              onClick={() => handleDeleteProduct(product._id)}
              colorScheme='pink'
            />
          </HStack>
        </Box>
      )}

      <Modal isOpen={isOpen} onClose={onClose}>
        <ModalOverlay />

        <ModalContent>
          <ModalHeader>Update Product</ModalHeader>
          <ModalCloseButton />
          <ModalBody>
            <VStack spacing={4}>
              <Input
                placeholder='Product Name'
                name='name'
                value={updatedProduct.name}
                onChange={(e) => setUpdatedProduct({ ...updatedProduct, name: e.target.value })}
              />
              <Input
                placeholder='Price'
                name='price'
                type='number'
                value={updatedProduct.price}
                onChange={(e) => setUpdatedProduct({ ...updatedProduct, price: e.target.value })}
              />
              <Input
                placeholder='Image URL'
                name='image'
                value={updatedProduct.image}
                onChange={(e) => setUpdatedProduct({ ...updatedProduct, image: e.target.value })}
              />
              <Textarea
                placeholder='Product Description'
                name='description'
                value={updatedProduct.description || ''}
                onChange={(e) => setUpdatedProduct({ ...updatedProduct, description: e.target.value })}
                rows={4}
              />
            </VStack>
          </ModalBody>

          <ModalFooter>
            <Button
              colorScheme='pink'
              mr={3}
              onClick={() => handleUpdateProduct(product._id, updatedProduct)}
            >
              Update
            </Button>
            <Button variant='ghost' onClick={onClose}>
              Cancel
            </Button>
          </ModalFooter>
        </ModalContent>
      </Modal>
    </Box>
  );
};
export default ProductCard;