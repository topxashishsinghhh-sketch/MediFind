import { useState, useEffect } from 'react';
import { Link } from 'react-router-dom';
import { productApi, Product } from '@/services/api';
import { useCart } from '@/context/CartContext';

type Tab = 'medicines' | 'healthcare';

export default function Catalogue({ type }: { type: Tab }) {
  const [tab, setTab] = useState<Tab>(type);
  const [search, setSearch] = useState('');
  const [products, setProducts] = useState<Product[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const { addItem } = useCart();

  useEffect(() => {
    let mounted = true;
    productApi.list({ category: tab }).then((res) => {
      if (!mounted) return;
      if (res.error) {
        setError(res.error);
        setLoading(false);
        return;
      }
      setProducts(res.data ?? []);
      setLoading(false);
    });
    return () => { mounted = false; };
  }, [tab]);

  const filtered = products.filter(
    (p) => p.name.toLowerCase().includes(search.toLowerCase()) || p.brand.toLowerCase().includes(search.toLowerCase())
  );

  if (loading) {
    return (
      <div className="p-4">
        <div className="flex flex-col md:flex-row justify-between items-end gap-4 mb-6">
          <div>
            <h1 className="text-2xl font-bold text-navy">{tab === 'medicines' ? 'Medicines' : 'Healthcare Products'}</h1>
            <p className="text-navy-600 text-sm">
              {tab === 'medicines' ? 'Prescription and over-the-counter medicines' : 'Personal care, wellness, and devices'}
            </p>
          </div>
          <div className="relative w-full md:w-96">
            <input
              type="text"
              placeholder="Search products..."
              className="w-full px-4 py-2 rounded-lg bg-white border border-navy-200 text-navy focus:ring-2 focus:ring-teal focus:outline-none"
              value={search}
              onChange={(e) => setSearch(e.target.value)}
            />
          </div>
        </div>

        <div className="flex gap-3 mb-6">
          <button
            onClick={() => setTab('medicines')}
            className={`px-4 py-2 rounded-lg text-sm font-medium transition ${tab === 'medicines' ? 'teal bg-teal text-white' : 'text-navy-700 bg-white border border-navy-200 hover:bg-navy-50'}`}
          >
            Medicines
          </button>
          <button
            onClick={() => setTab('healthcare')}
            className={`px-4 py-2 rounded-lg text-sm font-medium transition ${tab === 'healthcare' ? 'teal bg-teal text-white' : 'text-navy-700 bg-white border border-navy-200 hover:bg-navy-50'}`}
          >
            Healthcare
          </button>
        </div>

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

  if (error) {
    return (
      <div className="p-4 text-center py-12">
        <p className="text-red-600 text-lg">{error}</p>
        <button onClick={() => window.location.reload()} className="mt-4 teal bg-teal text-white px-4 py-2 rounded-lg text-sm font-medium hover:bg-teal-900 transition">
          Retry
        </button>
      </div>
    );
  }

  return (
    <div className="p-4">
      <div className="flex flex-col md:flex-row justify-between items-end gap-4 mb-6">
        <div>
          <h1 className="text-2xl font-bold text-navy">{tab === 'medicines' ? 'Medicines' : 'Healthcare Products'}</h1>
          <p className="text-navy-600 text-sm">
            {tab === 'medicines' ? 'Prescription and over-the-counter medicines' : 'Personal care, wellness, and devices'}
          </p>
        </div>
        <div className="relative w-full md:w-96">
          <input
            type="text"
            placeholder="Search products..."
            className="w-full px-4 py-2 rounded-lg bg-white border border-navy-200 text-navy focus:ring-2 focus:ring-teal focus:outline-none"
            value={search}
            onChange={(e) => setSearch(e.target.value)}
          />
        </div>
      </div>

      <div className="flex gap-3 mb-6">
        <button
          onClick={() => setTab('medicines')}
          className={`px-4 py-2 rounded-lg text-sm font-medium transition ${tab === 'medicines' ? 'teal bg-teal text-white' : 'text-navy-700 bg-white border border-navy-200 hover:bg-navy-50'}`}
        >
          Medicines
        </button>
        <button
          onClick={() => setTab('healthcare')}
          className={`px-4 py-2 rounded-lg text-sm font-medium transition ${tab === 'healthcare' ? 'teal bg-teal text-white' : 'text-navy-700 bg-white border border-navy-200 hover:bg-navy-50'}`}
        >
          Healthcare
        </button>
      </div>

      {filtered.length === 0 ? (
        <div className="text-center py-12">
          <p className="text-navy-500 text-lg">No products found</p>
        </div>
      ) : (
        <div className="grid grid-cols-1 sm:grid-cols-2 md:grid-cols-3 lg:grid-cols-4 gap-4">
          {filtered.map((p) => (
            <div
              key={p.id}
              className="bg-white rounded-xl border border-navy-100 p-4 shadow-sm hover:shadow-md transition"
            >
              <div className="h-32 bg-soft-grey rounded-lg mb-3 flex items-center justify-center relative">
                <img
                  src={p.imageUrl}
                  alt={p.name}
                  className="max-h-full max-w-full object-contain"
                />
                <span
                  className={`absolute top-2 left-2 px-2 py-1 rounded text-xs font-bold ${
                    p.prescriptionRequired ? 'bg-navy text-white' : 'bg-teal text-white'
                  }`}
                >
                  {p.prescriptionRequired ? 'Prescription Required' : 'OTC'}
                </span>
              </div>
              <h3 className="text-navy font-bold text-sm mb-1 line-clamp-2">{p.name}</h3>
              <p className="text-navy-600 text-xs mb-2">{p.brand}</p>
              <div className="flex justify-between items-baseline mb-3">
                <span className="teal font-bold">₹0.00</span>
                <span className="text-xs font-medium px-2 py-0.5 rounded bg-green-100 text-green-700">
                  In Stock
                </span>
              </div>
              <div className="flex gap-2 mt-3">
                <button
                  onClick={() => addItem(p)}
                  className="flex-1 teal bg-teal text-white py-2 rounded-lg text-sm font-medium hover:bg-teal-900 transition"
                >
                  Add to Cart
                </button>
                <Link
                  to={`/products/${p.id}`}
                  className="flex-1 text-navy-600 py-2 rounded-lg border border-navy-200 text-sm font-medium text-center hover:bg-navy-50 transition"
                >
                  View Details
                </Link>
              </div>
            </div>
          ))}
        </div>
      )}
    </div>
  );
}
