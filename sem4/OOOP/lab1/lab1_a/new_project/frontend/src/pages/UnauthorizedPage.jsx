import { Box, Button, Container, Heading, Text, VStack } from "@chakra-ui/react";
import { Link } from "react-router-dom";

/**
 * @file Unauthorized access page component
 * @module pages/UnauthorizedPage
 */

/**
 * UnauthorizedPage component is displayed when a user attempts to access
 * a route they don't have permission for.
 * 
 * @component
 * @returns {JSX.Element} - Rendered UnauthorizedPage component
 */

const UnauthorizedPage = () => {
  return (
    <Container maxW="container.md" py={10}>
      <VStack spacing={8} textAlign="center">
        <Heading 
          as="h1" 
          size="2xl"
          bgGradient="linear(to-r,rgb(233, 25, 132), rgb(152, 16, 186))"
          bgClip="text"
        >
          Access Denied
        </Heading>
        
        <Text fontSize="xl">
          You don't have permission to access this page.
        </Text>
        
        <Box>
          <Button 
            as={Link} 
            to="/" 
            colorScheme="pink"
          >
            Go Home
          </Button>
        </Box>
      </VStack>
    </Container>
  );
};

export default UnauthorizedPage;