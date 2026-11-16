import { useState, useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
import { useCart } from '../context/CartContext';
import { productApi, orderApi } from '../services/api';
import type { ProductWithBatches, CartItem } from '../services/api';

const ADDRESSES = [
  {
    id: 1,
    label: 'Home',
    street: '123 Main Street',
    city: 'New York',
    state: 'NY',
    postalCode: '10001',
    phone: '+1 234 567 8900',
    isDefault: true,
  },
];

export default function Checkout() {
  const { items, clear } = useCart();
  const navigate = useNavigate();
  const [step, setStep] = useState<'address' | 'payment' | 'confirm'>('address');
  const [paymentMethod, setPaymentMethod] = useState<'card' | 'upi' | 'cod'>('card');
  const [paymentSuccess, setPaymentSuccess] = useState<boolean | null>(null);
  const [orderId, setOrderId] = useState<number | null>(null);
  const [productDetails, setProductDetails] = useState<Map<number, ProductWithBatches>>(new Map());
  const [loading, setLoading] = useState(false);

  const selectedAddress = ADDRESSES.find((a) => a.isDefault) ?? ADDRESSES[0];

  useEffect(() => {
    if (items.length === 0) return;
    const fetchProducts = async () => {
      const map = new Map<number, ProductWithBatches>();
      for (const item of items) {
        const res = await productApi.get(item.productId);
        if (res.data) map.set(item.productId, res.data);
      }
      setProductDetails(map);
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

  const handlePlaceOrder = async () => {
    setLoading(true);
    try {
      const cartItems: { productId: number; quantity: number }[] = items.map((i: CartItem) => ({
        productId: i.productId,
        quantity: i.quantity,
      }));
      const res = await orderApi.create({
        customerId: 1,
        items: cartItems,
        deliveryAddress: `${selectedAddress.street}, ${selectedAddress.city}, ${selectedAddress.state} ${selectedAddress.postalCode}`,
      });
      if (res.data && 'id' in res.data) {
        setOrderId((res.data as any).id);
        setPaymentSuccess(true);
        clear();
      } else {
        setPaymentSuccess(false);
      }
    } catch {
      setPaymentSuccess(false);
    } finally {
      setLoading(false);
    }
  };

  const handlePayment = () => {
    setLoading(true);
    setTimeout(() => {
      setLoading(false);
      setPaymentSuccess(Math.random() < 0.8);
      if (paymentMethod === 'cod' || Math.random() < 0.8) {
        handlePlaceOrder();
      }
    }, 1500);
  };

  if (loading && step === 'confirm') {
    return (
      <div className="p-4 max-w-3xl mx-auto">
        <h1 className="text-2xl font-bold text-navy mb-6">Checkout</h1>
        <div className="bg-white rounded-xl border border-navy-100 p-6 shadow-sm text-center">
          <div className="flex gap-1 justify-center mb-4">
            {[...Array(5)].map((_, i) => (
              <div key={i} className="w-2 h-2 rounded-full bg-teal animate-pulse" style={{ animationDelay: `${i * 0.1}s` }}></div>
            ))}
          </div>
          <h2 className="text-navy font-bold text-lg mb-2">Processing Payment...</h2>
          <p className="text-navy-600 text-sm">Please wait while we process your order.</p>
        </div>
      </div>
    );
  }

  if (step === 'confirm' && orderId) {
    navigate(`/order-confirmation/${orderId}`);
    return null;
  }

  return (
    <div className="p-4 max-w-3xl mx-auto">
      <h1 className="text-2xl font-bold text-navy mb-6">Checkout</h1>

      {step === 'address' && (
        <div className="bg-white rounded-xl border border-navy-100 p-6 shadow-sm">
          <h2 className="text-navy font-bold text-lg mb-4">Delivery Address</h2>
          <div className="space-y-3">
            {ADDRESSES.map((addr) => (
              <div
                key={addr.id}
                className={`p-3 rounded-lg border flex items-center gap-3 cursor-pointer ${addr.isDefault ? 'border-teal bg-teal-50' : 'border-navy-100 bg-white hover:border-navy-300'}`}
              >
                <input type="radio" defaultChecked={addr.isDefault} className="w-4 h-4 text-teal" />
                <div>
                  <div className="flex justify-between">
                    <span className="font-bold text-navy">{addr.label}</span>
                    {addr.isDefault && <span className="text-xs bg-teal text-white px-2 py-0.5 rounded">Default</span>}
                  </div>
                  <p className="text-navy-600 text-sm">
                    {addr.street}, {addr.city}, {addr.state} {addr.postalCode}
                  </p>
                  <p className="text-navy-500 text-xs">{addr.phone}</p>
                </div>
              </div>
            ))}
          </div>
          <button
            onClick={() => setStep('payment')}
            className="mt-6 w-full teal bg-teal text-white py-3 rounded-lg font-bold hover:bg-teal-900 transition"
          >
            Continue to Payment
          </button>
        </div>
      )}

      {step === 'payment' && (
        <div className="bg-white rounded-xl border border-navy-100 p-6 shadow-sm">
          <h2 className="text-navy font-bold text-lg mb-4">Payment</h2>
          <p className="text-navy-600 text-sm mb-4">
            ⚠️ This is a simulated payment demonstration. No real card details are collected.
          </p>
          <div className="space-y-3 mb-6">
            <label className="block text-navy font-medium mb-2">Payment Method</label>
            <div className="space-y-2">
              {[
                { value: 'card' as const, label: 'Credit/Debit Card', icon: '💳' },
                { value: 'upi' as const, label: 'UPI / Wallet', icon: '📱' },
                { value: 'cod' as const, label: 'Cash on Delivery', icon: '💵' },
              ].map((m) => (
                <label
                  key={m.value}
                  className={`flex items-center gap-3 p-3 rounded-lg border cursor-pointer transition ${
                    paymentMethod === m.value ? 'border-teal bg-teal-50' : 'border-navy-100 bg-white hover:border-navy-300'
                  }`}
                >
                  <input
                    type="radio"
                    value={m.value}
                    checked={paymentMethod === m.value}
                    onChange={() => setPaymentMethod(m.value)}
                    className="w-4 h-4 text-teal"
                  />
                  <span className="text-navy font-medium">{m.icon} {m.label}</span>
                </label>
              ))}
            </div>
          </div>
          <div className="mb-6">
            <div className="flex justify-between text-navy-600 text-sm mb-3">
              <span>Order Total</span>
              <span className="font-bold text-navy text-lg">₹{total.toFixed(2)}</span>
            </div>
            <button
              onClick={handlePayment}
              disabled={loading}
              className="w-full teal bg-teal text-white py-3 rounded-lg font-bold text-lg hover:bg-teal-900 transition disabled:opacity-50"
            >
              {loading ? 'Processing...' : `Pay ₹${total.toFixed(2)}`}
            </button>
          </div>
        </div>
      )}

      {step === 'confirm' && paymentSuccess === null && !orderId && (
        <div className="bg-white rounded-xl border border-navy-100 p-6 shadow-sm text-center">
          <h2 className="text-navy font-bold text-lg mb-2">Placing Order...</h2>
          <p className="text-navy-600 text-sm">Please wait while we place your order.</p>
        </div>
      )}
    </div>
  );
}
