import { useState } from "react";
import {
  Box,
  Button,
  Container,
  FormControl,
  FormLabel,
  Heading,
  Input,
  Text,
  useColorModeValue,
  useToast,
  VStack,
  Link as ChakraLink,
  Spinner,
} from "@chakra-ui/react";
import { Link, useNavigate } from "react-router-dom";
import { useAuthStore } from "../store/auth";

/**
 * @file Login page component
 * @module pages/LoginPage
 */

/**
 * LoginPage component provides a form for users to sign in to their accounts.
 * Handles form validation and authentication.
 * 
 * @component
 * @returns {JSX.Element} - Rendered LoginPage component
 */

const LoginPage = () => {
  const [formData, setFormData] = useState({
    email: "",
    password: "",
  });
  
  const toast = useToast();
  const navigate = useNavigate();
  const { login, loading } = useAuthStore();
  
  const handleChange = (e) => {
    setFormData({ ...formData, [e.target.name]: e.target.value });
  };
  
  const handleSubmit = async (e) => {
    e.preventDefault();
    
    if (!formData.email || !formData.password) {
      toast({
        title: "Error",
        description: "Please fill in all fields",
        status: "error",
        duration: 3000,
        isClosable: true,
      });
      return;
    }
    
    const { success, message } = await login(formData);
    
    if (success) {
      toast({
        title: "Success",
        description: "Logged in successfully",
        status: "success",
        duration: 3000,
        isClosable: true,
      });
      // redirect to the main page and add forced page refresh
      window.location.href = '/';
    } else {
      toast({
        title: "Error",
        description: message,
        status: "error",
        duration: 3000,
        isClosable: true,
      });
    }
  };
  
  return (
    <Container maxW="container.sm" py={10}>
      <VStack spacing={8}>
        <Heading 
          as="h1" 
          size="2xl" 
          textAlign="center"
          bgGradient="linear(to-r,rgb(233, 25, 132), rgb(152, 16, 186))"
          bgClip="text"
        >
          Login
        </Heading>
        
        <Box
          w="full"
          bg={useColorModeValue("white", "rgb(40, 4, 30)")}
          p={6}
          rounded="lg"
          shadow="md"
          as="form"
          onSubmit={handleSubmit}
        >
          <VStack spacing={4}>
            <FormControl isRequired>
              <FormLabel>Email</FormLabel>
              <Input
                name="email"
                type="email"
                value={formData.email}
                onChange={handleChange}
                placeholder="Enter your email"
                disabled={loading}
              />
            </FormControl>
            
            <FormControl isRequired>
              <FormLabel>Password</FormLabel>
              <Input
                name="password"
                type="password"
                value={formData.password}
                onChange={handleChange}
                placeholder="Enter your password"
                disabled={loading}
              />
            </FormControl>
            
            <Button
              colorScheme="pink"
              type="submit"
              w="full"
              mt={4}
              isLoading={loading}
              loadingText="Logging in"
            >
              {loading ? <Spinner size="sm" /> : "Login"}
            </Button>
            
            <Text mt={4}>
              Don't have an account?{" "}
              <ChakraLink as={Link} to="/register" color="pink.500">
                Register
              </ChakraLink>
            </Text>
          </VStack>
        </Box>
      </VStack>
    </Container>
  );
};

export default LoginPage;