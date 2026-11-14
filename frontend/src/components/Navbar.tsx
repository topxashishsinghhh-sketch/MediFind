import { Link } from 'react-router-dom';
import { useCart } from '../context/CartContext';

export default function Navbar() {
  const { itemCount } = useCart();

  return (
    <nav className="navy px-4 py-3 flex justify-between items-center shadow-sm">
      <div className="flex items-center gap-2">
        <div className="w-8 h-8 rounded-full teal flex items-center justify-center text-white font-bold text-lg">
          M
        </div>
        <Link to="/" className="text-xl font-bold text-white no-underline">
          MediFind
        </Link>
      </div>
      <div className="hidden md:flex gap-6">
        <Link to="/" className="text-teal-200 hover:text-white transition">
          Home
        </Link>
        <Link to="/medicines" className="text-teal-200 hover:text-white transition">
          Medicines
        </Link>
        <Link to="/healthcare" className="text-teal-200 hover:text-white transition">
          Healthcare
        </Link>
        <Link to="/cart" className="text-teal-200 hover:text-white transition">
          Cart
        </Link>
      </div>
      <div className="flex items-center gap-4">
        <Link
          to="/cart"
          className="relative p-2 text-teal-200 hover:text-white transition"
        >
          🛒
          {itemCount > 0 && (
            <span className="absolute top-1 right-1 w-4 h-4 rounded-full bg-red-500 text-white text-xs flex items-center justify-center">
              {itemCount}
            </span>
          )}
        </Link>
        <button className="teal bg-teal text-white px-4 py-2 rounded-lg text-sm font-medium hover:bg-teal-900 transition">
          Login
        </button>
      </div>
    </nav>
  );
}
