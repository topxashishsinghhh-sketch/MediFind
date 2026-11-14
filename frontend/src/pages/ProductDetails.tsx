import { useState, useEffect } from 'react';
import { useParams } from 'react-router-dom';
import { Link } from 'react-router-dom';
import { productApi, ProductWithBatches, Batch } from '@/services/api';

export default function ProductDetails() {
  const { id } = useParams<{ id: string }>();
  const productId = parseInt(id || '1', 10);
  const [product, setProduct] = useState<ProductWithBatches | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [quantity, setQuantity] = useState(1);

  useEffect(() => {
    let mounted = true;
    productApi.get(productId).then((res) => {
      if (!mounted) return;
      if (res.error) {
        setError(res.error);
        setLoading(false);
        return;
      }
      setProduct(res.data ?? null);
      setLoading(false);
    });
    return () => { mounted = false; };
  }, [productId]);

  if (loading) {
    return (
      <div className="p-4 max-w-5xl mx-auto">
        <Link to="/medicines" className="text-teal text-sm mb-4 inline-block underline hover:text-teal-900">
          ← Back to Medicines
        </Link>
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

  if (error || !product) {
    return (
      <div className="p-4 max-w-5xl mx-auto text-center py-12">
        <Link to="/medicines" className="text-teal text-sm mb-4 inline-block underline hover:text-teal-900">
          ← Back to Medicines
        </Link>
        <p className="text-red-600 text-lg">{error || 'Product not found'}</p>
      </div>
    );
  }

  const batches: Batch[] = product.batches;
  const totalAvailable = batches.reduce((sum, b) => sum + b.availableStock, 0);
  const maxQty = Math.min(totalAvailable, 10);
  const inStock = totalAvailable > 0;

  return (
    <div className="p-4 max-w-5xl mx-auto">
      <Link to="/medicines" className="text-teal text-sm mb-4 inline-block underline hover:text-teal-900">
        ← Back to Medicines
      </Link>

      <div className="grid grid-cols-1 lg:grid-cols-2 gap-8">
        <div className="bg-white rounded-xl border border-navy-100 p-6 shadow-sm">
          <div className="h-64 bg-soft-grey rounded-lg mb-4 flex items-center justify-center">
            <img
              src={product.imageUrl}
              alt={product.name}
              className="max-h-full max-w-full object-contain"
            />
          </div>
          <span
            className={`inline-block px-3 py-1 rounded text-xs font-bold mb-2 ${
              product.prescriptionRequired ? 'bg-navy text-white' : 'bg-teal text-white'
            }`}
          >
            {product.prescriptionRequired ? 'Prescription Required' : 'Over the Counter'}
          </span>
        </div>

        <div className="space-y-6">
          <div>
            <div className="flex justify-between items-baseline mb-2">
              <h1 className="text-2xl font-bold text-navy">{product.name}</h1>
              <span className="teal text-xl font-bold">₹{(batches[0]?.sellingPrice ?? 0).toFixed(2)}</span>
            </div>
            <p className="text-navy-600 text-sm">{product.brand} • {product.manufacturer}</p>
            <div className="mt-4 flex gap-4 text-sm text-navy-700">
              <span>Strength: {product.strength}</span>
              <span>Form: {product.dosageForm}</span>
              <span>Category: {product.category}</span>
            </div>
          </div>

          <div className="bg-white rounded-xl border border-navy-100 p-6 shadow-sm">
            <h3 className="text-navy font-bold text-lg mb-3">Description</h3>
            <p className="text-navy-600 text-sm leading-relaxed">{product.description}</p>
          </div>

          <div className="bg-white rounded-xl border border-navy-100 p-6 shadow-sm">
            <h3 className="text-navy font-bold text-lg mb-3">Available Batches</h3>
            <div className="space-y-2">
              {batches.map((b: Batch) => (
                <div key={b.id} className="flex justify-between items-center p-3 bg-soft-grey rounded-lg text-sm">
                  <span className="text-navy-700">Batch #{b.id}</span>
                  <div className="text-right">
                    <div className="text-navy-600">Expires: {b.expiryDate}</div>
                    <div className="text-teal font-bold">Available: {b.availableStock}</div>
                  </div>
                </div>
              ))}
            </div>
          </div>

          <div className="bg-white rounded-xl border border-navy-100 p-6 shadow-sm">
            <div className="flex items-center gap-4">
              <label className="text-navy font-medium">Quantity:</label>
              <select
                value={quantity}
                onChange={(e) => setQuantity(Math.min(parseInt(e.target.value, 10), maxQty || 1))}
                className="px-3 py-2 rounded bg-white border border-navy-200 text-navy"
                disabled={!inStock}
              >
                {[...Array(maxQty || 1)].map((_, i) => i + 1).map((q) => (
                  <option key={q} value={q}>
                    {q}
                  </option>
                ))}
              </select>
            </div>
            {!inStock ? (
              <button className="w-full bg-navy-200 text-navy-500 py-3 rounded-lg font-bold text-lg cursor-not-allowed">
                Out of Stock
              </button>
            ) : (
              <button className="w-full teal bg-teal text-white py-3 rounded-lg font-bold text-lg hover:bg-teal-900 transition">
                Add to Cart
              </button>
            )}
          </div>
        </div>
      </div>
    </div>
  );
}
