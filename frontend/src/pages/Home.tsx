import { useState } from 'react';
import { Link } from 'react-router-dom';

const CATEGORIES = [
  { id: 'all', name: 'All Products' },
  { id: 'medicine', name: 'Medicines' },
  { id: 'healthcare', name: 'Healthcare' },
];

const FEATURED = [
  {
    id: 1,
    name: 'Paracetamol 500mg',
    brand: 'MediCare',
    category: 'medicine',
    price: 4.99,
    image: 'https://via.placeholder.com/150',
    prescriptionRequired: false,
    rating: 4.8,
  },
  {
    id: 2,
    name: 'Vitamin C 1000mg',
    brand: 'HealthPlus',
    category: 'healthcare',
    price: 12.5,
    image: 'https://via.placeholder.com/150',
    prescriptionRequired: false,
    rating: 4.6,
  },
  {
    id: 3,
    name: 'Amoxicillin 250mg',
    brand: 'PharmaCo',
    category: 'medicine',
    price: 18.99,
    image: 'https://via.placeholder.com/150',
    prescriptionRequired: true,
    rating: 4.9,
  },
  {
    id: 4,
    name: 'Digital Blood Pressure Monitor',
    brand: 'MediTech',
    category: 'healthcare',
    price: 45.0,
    image: 'https://via.placeholder.com/150',
    prescriptionRequired: false,
    rating: 4.7,
  },
];

export default function Home() {
  const [searchQuery, setSearchQuery] = useState('');
  const [selectedCategory, setSelectedCategory] = useState('all');

  return (
    <div className="p-4">
      <header className="mb-8">
        <h1 className="text-3xl font-bold text-navy mb-2">MediFind</h1>
        <p className="text-navy-700">Your trusted online pharmacy and healthcare platform</p>
      </header>

      <div className="mb-8">
        <div className="relative">
          <input
            type="text"
            placeholder="Search medicines, healthcare products..."
            className="w-full px-4 py-3 rounded-xl bg-white border border-navy-200 text-navy focus:ring-2 focus:ring-teal focus:outline-none text-lg"
            value={searchQuery}
            onChange={(e) => setSearchQuery(e.target.value)}
          />
        </div>
      </div>

      <div className="mb-8">
        <div className="flex gap-3 overflow-x-auto pb-2">
          {CATEGORIES.map((cat) => (
            <button
              key={cat.id}
              onClick={() => setSelectedCategory(cat.id)}
              className={`px-5 py-2 rounded-full whitespace-nowrap text-sm font-medium transition ${
                selectedCategory === cat.id
                  ? 'teal bg-teal text-white'
                  : 'text-navy-700 bg-white border border-navy-200 hover:bg-navy-50'
              }`}
            >
              {cat.name}
            </button>
          ))}
        </div>
      </div>

      <section className="mb-8">
        <h2 className="text-xl font-bold text-navy mb-4">Featured Products</h2>
        <div className="grid grid-cols-1 sm:grid-cols-2 md:grid-cols-3 lg:grid-cols-4 gap-4">
          {FEATURED.map((p) => (
            <div
              key={p.id}
              className="bg-white rounded-xl border border-navy-100 p-4 shadow-sm hover:shadow-md transition"
            >
              <div className="h-32 bg-soft-grey rounded-lg mb-3 flex items-center justify-center">
                <span className="text-navy-500 text-sm">{p.prescriptionRequired ? 'Rx' : 'OTC'}</span>
              </div>
              <h3 className="text-navy font-bold text-sm mb-1">{p.name}</h3>
              <p className="text-navy-600 text-xs mb-2">{p.brand}</p>
              <div className="flex justify-between items-baseline">
                <span className="teal font-bold">₹{p.price}</span>
                <span className="text-navy-500 text-xs">{p.rating} ⭐</span>
              </div>
              <Link
                to={`/products/${p.id}`}
                className="block mt-3 text-teal text-sm font-medium underline hover:text-teal-900"
              >
                View Details
              </Link>
            </div>
          ))}
        </div>
      </section>

      <section>
        <h2 className="text-xl font-bold text-navy mb-4">Categories</h2>
        <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
          {[
            { name: 'Medicines', icon: '💊', path: '/medicines' },
            { name: 'Healthcare', icon: '🩺', path: '/healthcare' },
            { name: 'Personal Care', icon: '🧴', path: '/healthcare' },
            { name: 'Wellness', icon: '🌿', path: '/healthcare' },
          ].map((c) => (
            <div
              key={c.name}
              className="bg-white rounded-xl border border-navy-100 p-4 text-center hover:shadow-md transition cursor-pointer"
              onClick={() => {}}
            >
              <span className="text-2xl mb-2 block">{c.icon}</span>
              <h3 className="text-navy font-bold text-sm">{c.name}</h3>
            </div>
          ))}
        </div>
      </section>
    </div>
  );
}
