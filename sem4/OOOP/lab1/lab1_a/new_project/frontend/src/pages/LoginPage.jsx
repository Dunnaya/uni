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

const LoginPage = () => {
  const [formData, setFormData] = useState({
    email: "",
    password: "",
  });
  const { login, loading } = useAuthStore();
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
    
    const { success, message } = await login(formData);
    
    if (success) {
      toast({
        title: "Successful login",
        status: "success",
        duration: 3000,
        isClosable: true,
      });
      navigate("/");
    } else {
      toast({
        title: "Login error",
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
          Login to your account
        </Heading>

        <Box w="full" bg={bgColor} p={8} rounded="lg" shadow="md">
          <form onSubmit={handleSubmit}>
            <VStack spacing={4}>
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
                Login
              </Button>
            </VStack>
          </form>
        </Box>

        <Text>
            Don't have an account yet?{" "}
          <Link to="/register">
            <Text as="span" color="pink.500" fontWeight="bold">
                Register
            </Text>
          </Link>
        </Text>
      </VStack>
    </Container>
  );
};

export default LoginPage;