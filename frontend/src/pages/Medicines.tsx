import { useState, useEffect } from 'react';
import { Link } from 'react-router-dom';
import { productApi } from '../services/api';
import type { Product } from '../services/api';
import { useCart } from '../context/CartContext';

type Tab = 'medicines' | 'healthcare';

export default function Catalogue({ type = 'medicines' }: { type?: Tab }) {
  const [tab, setTab] = useState<Tab>(type);
  const [search, setSearch] = useState('');
  const [products, setProducts] = useState<Product[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const { addItem } = useCart();

  useEffect(() => {
    let mounted = true;
    productApi.list({ category: tab }).then((res: { data?: Product[]; error?: string }) => {
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
      </div>
    );
  }

  return (
    <div className="p-4 max-w-7xl mx-auto">
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

      <div className="grid grid-cols-1 sm:grid-cols-2 md:grid-cols-3 lg:grid-cols-4 gap-6">
        {filtered.map((product) => (
          <div key={product.id} className="bg-white rounded-xl border border-navy-100 p-4 shadow-sm hover:shadow-md transition">
            <div className="h-40 bg-soft-grey rounded-lg mb-3 flex items-center justify-center">
              <img
                src={product.imageUrl}
                alt={product.name}
                className="max-h-full max-w-full object-contain"
              />
            </div>
            <span
              className={`inline-block px-2 py-0.5 rounded text-xs font-bold mb-2 ${
                product.prescriptionRequired ? 'bg-navy text-white' : 'bg-teal text-white'
              }`}
            >
              {product.prescriptionRequired ? 'Rx Required' : 'OTC'}
            </span>
            <h3 className="text-navy font-bold text-sm mb-1">{product.name}</h3>
            <p className="text-navy-500 text-xs mb-3">{product.brand} • {product.manufacturer}</p>
            <div className="flex justify-between items-center mt-auto">
              <Link
                to={`/products/${product.id}`}
                className="text-teal text-sm font-medium underline hover:text-teal-900"
              >
                Details
              </Link>
              <button
                onClick={() => addItem(product, 1)}
                className="teal bg-teal text-white px-3 py-1 rounded text-sm font-medium hover:bg-teal-900 transition"
              >
                Add to Cart
              </button>
            </div>
          </div>
        ))}
      </div>
    </div>
  );
}
