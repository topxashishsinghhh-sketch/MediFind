import { useState, useEffect } from 'react';
import { Link, useNavigate } from 'react-router-dom';
import { useCart } from '../context/CartContext';
import { productApi } from '../services/api';
import type { ProductWithBatches, CartItem, Batch } from '../services/api';

export default function Cart() {
  const { items, updateQuantity, removeItem, itemCount } = useCart();
  const navigate = useNavigate();
  const [productDetails, setProductDetails] = useState<Map<number, ProductWithBatches>>(new Map());
  const [loading, setLoading] = useState(false);

  useEffect(() => {
    if (items.length === 0) return;
    setLoading(true);
    const fetchProducts = async () => {
      const map = new Map<number, ProductWithBatches>();
      for (const item of items) {
        const res = await productApi.get(item.productId);
        if (res.data) map.set(item.productId, res.data);
      }
      setProductDetails(map);
      setLoading(false);
    };
    fetchProducts();
  }, [items]);

  const subtotal = Array.from(productDetails.entries()).reduce((sum, [pid, p]) => {
    const item = items.find((i: CartItem) => i.productId === pid);
    const price = p.batches?.[0]?.sellingPrice ?? 0;
    return sum + price * (item?.quantity ?? 0);
  }, 0);
  const deliveryFee = 5.0;
  const total = subtotal + deliveryFee;

  if (loading) {
    return (
      <div className="p-4 max-w-4xl mx-auto">
        <h1 className="text-2xl font-bold text-navy mb-6">Shopping Cart</h1>
        <div className="flex justify-center py-12">
          <div className="flex gap-1">
            {[...Array(5)].map((_, i) => (
              <div key={i} className="w-2 h-2 rounded-full bg-teal animate-pulse" style={{ animationDelay: `${i * 0.1}s` }}></div>
            ))}
          </div>
        </div>
      </div>
    );
  }

  if (items.length === 0) {
    return (
      <div className="p-4 max-w-4xl mx-auto">
        <h1 className="text-2xl font-bold text-navy mb-6">Shopping Cart</h1>
        <div className="text-center py-12">
          <p className="text-navy-500 text-lg">Your cart is empty</p>
          <Link to="/medicines" className="teal bg-teal text-white px-6 py-2 rounded-lg font-bold hover:bg-teal-900 transition">
            Browse Medicines
          </Link>
        </div>
      </div>
    );
  }

  return (
    <div className="p-4 max-w-4xl mx-auto">
      <h1 className="text-2xl font-bold text-navy mb-6">Shopping Cart</h1>

      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
        <div className="lg:col-span-2">
          <div className="space-y-4">
            {items.map((item: CartItem) => {
              const product = productDetails.get(item.productId);
              if (!product) return null;
              const price = product.batches?.[0]?.sellingPrice ?? 0;
              const inStock = product.batches?.some((b: Batch) => b.availableStock > 0) ?? false;
              return (
                <div key={item.productId} className="bg-white rounded-xl border border-navy-100 p-4 flex gap-4 shadow-sm">
                  <div className="w-20 h-20 bg-soft-grey rounded-lg flex-shrink-0 flex items-center justify-center">
                    <img
                      src={product.imageUrl}
                      alt={product.name}
                      className="max-h-full max-w-full object-contain"
                    />
                  </div>
                  <div className="flex-1">
                    <div className="flex justify-between">
                      <h3 className="text-navy font-bold">{product.name}</h3>
                      <span className="text-navy-600">₹{price.toFixed(2)}</span>
                    </div>
                    <p className="text-navy-500 text-sm">{product.brand}</p>
                    <div className="mt-3 flex items-center gap-3">
                      <button
                        onClick={() => updateQuantity(item.productId, item.quantity - 1)}
                        disabled={!inStock}
                        className="w-8 h-8 rounded border border-navy-200 hover:bg-navy-50 disabled:opacity-50"
                      >
                        −
                      </button>
                      <span className="font-medium text-navy">{item.quantity}</span>
                      <button
                        onClick={() => updateQuantity(item.productId, item.quantity + 1)}
                        disabled={!inStock}
                        className="w-8 h-8 rounded border border-navy-200 hover:bg-navy-50 disabled:opacity-50"
                      >
                        +
                      </button>
                    </div>
                    {product.prescriptionRequired && (
                      <p className="mt-2 text-xs text-orange-600">Prescription required for this item</p>
                    )}
                    {!inStock && (
                      <p className="mt-2 text-xs text-red-600">Out of stock</p>
                    )}
                  </div>
                  <button
                    onClick={() => removeItem(item.productId)}
                    className="text-navy-500 hover:text-navy-800"
                  >
                    Remove
                  </button>
                </div>
              );
            })}
          </div>
        </div>

        <div className="bg-white rounded-xl border border-navy-100 p-6 shadow-lg sticky top-4">
          <h3 className="text-navy font-bold text-lg mb-4">Order Summary</h3>
          <div className="space-y-3 mb-6">
            <div className="flex justify-between text-navy-600 text-sm">
              <span>Subtotal ({itemCount} items)</span>
              <span>₹{subtotal.toFixed(2)}</span>
            </div>
            <div className="flex justify-between text-navy-600 text-sm">
              <span>Delivery Fee</span>
              <span>₹{deliveryFee.toFixed(2)}</span>
            </div>
            <div className="flex justify-between text-navy font-bold text-lg border-t border-navy-100 pt-3">
              <span>Total</span>
              <span>₹{total.toFixed(2)}</span>
            </div>
          </div>

          <div className="space-y-3">
            <p className="text-navy-500 text-sm mb-2">Prescription items will be reviewed before dispatch.</p>
            <button
              onClick={() => navigate('/checkout')}
              className="w-full teal bg-teal text-white py-3 rounded-lg font-bold text-lg hover:bg-teal-900 transition"
            >
              Proceed to Checkout
            </button>
          </div>
        </div>
      </div>
    </div>
  );
}
