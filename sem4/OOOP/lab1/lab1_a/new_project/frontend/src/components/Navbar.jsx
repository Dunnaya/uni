import { 
	Button, 
	Container, 
	Flex, 
	HStack, 
	Text, 
	useColorMode, 
	Menu,
	MenuButton,
	MenuList,
	MenuItem,
	Avatar,
	useToast,
	Box,
	Badge
  } from "@chakra-ui/react";
  import { Link } from "react-router-dom";
  
  import { PlusSquareIcon, ChevronDownIcon } from "@chakra-ui/icons";
  import { IoMoon } from "react-icons/io5";
  import { LuSun } from "react-icons/lu";
  import { FiShoppingCart } from "react-icons/fi"; // Import cart icon
  import { useAuthStore } from "../store/auth";
  import { useCartStore } from "../store/cart"; // Import cart store
  
  const Navbar = () => {
	const { colorMode, toggleColorMode } = useColorMode();
	const { isAuthenticated, isAdmin, user, logout } = useAuthStore();
	const { totalItems } = useCartStore(); // Get total items from cart store
	const toast = useToast();
	
	const handleLogout = () => {
	  logout();
	  toast({
		title: "Logged out",
		description: "You have been logged out successfully",
		status: "success",
		duration: 3000,
		isClosable: true,
	  });
	};
  
	return (
	  <Container maxW={"1140px"} px={4}>
		<Flex
		  h={16}
		  alignItems={"center"}
		  justifyContent={"space-between"}
		  flexDir={{
			base: "column",
			sm: "row",
		  }}
		>
		  <Text
			fontSize={{ base: "22", sm: "28" }}
			fontWeight={"bold"}
			textTransform={"uppercase"}
			textAlign={"center"}
			bgGradient={"linear(to-r,rgb(233, 25, 132), rgb(152, 16, 186))"}
			bgClip={"text"}
		  >
			<Link to={"/"}>♡ Product Store ♡</Link>
		  </Text>
  
		  <HStack spacing={2} alignItems={"center"}>
			{isAuthenticated && isAdmin && (
			  <Link to={"/create"}>
				<Button>
				  <PlusSquareIcon fontSize={20} />
				</Button>
			  </Link>
			)}
			
			{/* Cart Button with Badge */}
			<Box position="relative" display="inline-block">
			  <Button 
				as={Link} 
				to={isAuthenticated ? "/cart" : "/login"} 
				aria-label="Shopping Cart"
			  >
				<FiShoppingCart fontSize={20} />
			  </Button>
			  {isAuthenticated && totalItems > 0 && (
				<Badge 
				  colorScheme="pink" 
				  borderRadius="full" 
				  position="absolute" 
				  top="-5px" 
				  right="-5px"
				>
				  {totalItems}
				</Badge>
			  )}
			</Box>
			
			<Button onClick={toggleColorMode}>
			  {colorMode === "light" ? <IoMoon /> : <LuSun size='20' />}
			</Button>
			
			{!isAuthenticated ? (
			  <HStack>
				<Button as={Link} to="/login" colorScheme="pink" variant="outline">
				  Login
				</Button>
				<Button as={Link} to="/register" colorScheme="pink">
				  Register
				</Button>
			  </HStack>
			) : (
			  <Menu>
				<MenuButton as={Button} rightIcon={<ChevronDownIcon />}>
				  <HStack>
					<Avatar size="xs" name={user?.name} />
					<Text display={{ base: "none", md: "block" }}>{user?.name}</Text>
				  </HStack>
				</MenuButton>
				<MenuList>
				  {isAdmin && (
					<Text px={3} py={1} fontWeight="bold" color="gray.500">
					  Admin
					</Text>
				  )}
				  <MenuItem onClick={handleLogout}>Logout</MenuItem>
				</MenuList>
			  </Menu>
			)}
		  </HStack>
		</Flex>
	  </Container>
	);
  };
  export default Navbar;