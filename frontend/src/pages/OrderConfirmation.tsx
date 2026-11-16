import { useState, useEffect } from 'react';
import { useParams, Link } from 'react-router-dom';
import { orderApi } from '../services/api';
import type { Order, OrderItem } from '../services/api';

export default function OrderConfirmation() {
  const { id } = useParams<{ id: string }>();
  const orderId = parseInt(id || '1', 10);
  const [order, setOrder] = useState<Order | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    let mounted = true;
    orderApi.get(orderId).then((res: { data?: Order; error?: string }) => {
      if (!mounted) return;
      if (res.error) {
        setError(res.error);
        setLoading(false);
        return;
      }
      setOrder(res.data ?? null);
      setLoading(false);
    });
    return () => { mounted = false; };
  }, [orderId]);

  if (loading) {
    return (
      <div className="p-4 max-w-md mx-auto">
        <div className="bg-white rounded-xl border border-navy-100 p-8 shadow-sm text-center">
          <div className="flex gap-1 justify-center mb-4">
            {[...Array(5)].map((_, i) => (
              <div key={i} className="w-2 h-2 rounded-full bg-teal animate-pulse" style={{ animationDelay: `${i * 0.1}s` }}></div>
            ))}
          </div>
          <h1 className="text-2xl font-bold text-navy mb-2">Placing Order...</h1>
        </div>
      </div>
    );
  }

  if (error || !order) {
    return (
      <div className="p-4 max-w-md mx-auto">
        <div className="bg-white rounded-xl border border-navy-100 p-8 shadow-sm text-center">
          <div className="w-16 h-16 bg-red-100 rounded-full flex items-center justify-center mx-auto mb-6">
            ❌
          </div>
          <h1 className="text-2xl font-bold text-navy mb-2">Order Failed</h1>
          <p className="text-navy-600 text-sm mb-6">{error || 'Order not found'}</p>
          <Link to="/medicines" className="teal bg-teal text-white px-6 py-2 rounded-lg font-bold hover:bg-teal-900 transition">
            Browse Medicines
          </Link>
        </div>
      </div>
    );
  }

  return (
    <div className="p-4 max-w-md mx-auto">
      <div className="bg-white rounded-xl border border-navy-100 p-8 shadow-sm text-center">
        <div className="w-16 h-16 bg-green-100 rounded-full flex items-center justify-center mx-auto mb-6">
          ✅
        </div>
        <h1 className="text-2xl font-bold text-navy mb-2">Order Confirmed</h1>
        <p className="text-navy-600 text-sm mb-6">
          Thank you for your order! Your order #{order.id} has been placed successfully.
        </p>
        <div className="space-y-3 text-left">
          <div className="flex justify-between text-navy-600">
            <span>Order Number:</span>
            <span className="font-bold text-navy">#{order.id}</span>
          </div>
          <div className="flex justify-between text-navy-600">
            <span>Status:</span>
            <span className={`font-bold capitalize ${order.status === 'paid' ? 'teal' : 'navy'}`}>
              {order.status}
            </span>
          </div>
          <div className="flex justify-between text-navy-600">
            <span>Payment:</span>
            <span className="font-bold capitalize">{order.paymentStatus}</span>
          </div>
          <div className="flex justify-between text-navy-600 border-t border-navy-100 pt-3">
            <span>Total:</span>
            <span className="font-bold text-navy">₹{order.totalAmount.toFixed(2)}</span>
          </div>
          <div className="flex justify-between text-navy-600">
            <span>Delivery Address:</span>
            <span className="font-bold text-navy text-sm">{order.deliveryAddress}</span>
          </div>
        </div>
        <div className="mt-6">
          {order.items && order.items.length > 0 && (
            <div className="mb-4">
              <h3 className="text-navy font-bold text-sm mb-2">Items</h3>
              <div className="space-y-2">
                {order.items.map((item: OrderItem) => (
                  <div key={item.id} className="flex justify-between text-sm text-navy-600">
                    <span>Product #{item.productId} × {item.quantity}</span>
                    <span>₹{item.totalPrice.toFixed(2)}</span>
                  </div>
                ))}
              </div>
            </div>
          )}
          <button className="teal bg-teal text-white px-6 py-2 rounded-lg font-bold hover:bg-teal-900 transition">
            Track Order
          </button>
        </div>
      </div>
    </div>
  );
}
