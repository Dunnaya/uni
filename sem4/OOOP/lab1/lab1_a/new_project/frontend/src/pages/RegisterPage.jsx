import { useState } from "react";
import { useNavigate, Link } from "react-router-dom";
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
} from "@chakra-ui/react";
import { useAuthStore } from "../store/user";

const RegisterPage = () => {
  const [formData, setFormData] = useState({
    name: "",
    email: "",
    password: "",
  });
  const { register, loading } = useAuthStore();
  const toast = useToast();
  const navigate = useNavigate();
  const bgColor = useColorModeValue("white", "rgb(40, 4, 30)");

  const handleChange = (e) => {
    setFormData({
      ...formData,
      [e.target.name]: e.target.value,
    });
  };

  const handleSubmit = async (e) => {
    e.preventDefault();
    
    if (formData.password.length < 6) {
      toast({
        title: "Registration error",
        description: "Password must be at least 6 characters long",
        status: "error",
        duration: 3000,
        isClosable: true,
      });
      return;
    }
    
    const { success, message } = await register(formData);
    
    if (success) {
      toast({
        title: "Registration successful",
        description: "Now you can use the service",
        status: "success",
        duration: 3000,
        isClosable: true,
      });
      navigate("/");
    } else {
      toast({
        title: "Registration error",
        description: message,
        status: "error",
        duration: 3000,
        isClosable: true,
      });
    }
  };

  return (
    <Container maxW="container.sm" py={12}>
      <VStack spacing={8}>
        <Heading
          as="h1"
          size="2xl"
          textAlign="center"
          bgGradient="linear(to-r, rgb(233, 25, 132), rgb(152, 16, 186))"
          bgClip="text"
        >
          Registration
        </Heading>

        <Box w="full" bg={bgColor} p={8} rounded="lg" shadow="md">
          <form onSubmit={handleSubmit}>
            <VStack spacing={4}>
              <FormControl id="name" isRequired>
                <FormLabel>Name</FormLabel>
                <Input
                  type="text"
                  name="name"
                  value={formData.name}
                  onChange={handleChange}
                  placeholder="Your name"
                />
              </FormControl>
              
              <FormControl id="email" isRequired>
                <FormLabel>Email</FormLabel>
                <Input
                  type="email"
                  name="email"
                  value={formData.email}
                  onChange={handleChange}
                  placeholder="your@email.com"
                />
              </FormControl>
              
              <FormControl id="password" isRequired>
                <FormLabel>Password</FormLabel>
                <Input
                  type="password"
                  name="password"
                  value={formData.password}
                  onChange={handleChange}
                  placeholder="••••••••"
                />
              </FormControl>

              <Button
                type="submit"
                colorScheme="pink"
                w="full"
                mt={4}
                isLoading={loading}
              >
                Register
              </Button>
            </VStack>
          </form>
        </Box>

        <Text>
        Already have an account?{" "}
          <Link to="/login">
            <Text as="span" color="pink.500" fontWeight="bold">
              Login
            </Text>
          </Link>
        </Text>
      </VStack>
    </Container>
  );
};

export default RegisterPage;