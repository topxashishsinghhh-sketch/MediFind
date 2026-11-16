export type UserRole = 'customer' | 'employee' | 'admin';

export interface User {
  id: number;
  email: string;
  role: UserRole;
  fullName: string;
  phone: string;
  createdAt: string;
}

export interface Product {
  id: number;
  name: string;
  brand: string;
  category: 'medicine' | 'healthcare';
  manufacturer: string;
  strength: string;
  dosageForm: string;
  prescriptionRequired: boolean;
  description: string;
  imageUrl: string;
  isArchived: boolean;
  createdAt: string;
}

export interface Batch {
  id: number;
  productId: number;
  manufacturingDate: string;
  expiryDate: string;
  purchasePrice: number;
  sellingPrice: number;
  physicalQuantity: number;
  reservedQuantity: number;
  blockedQuantity: number;
  supplierId: number;
  isExpired: boolean;
  isBlocked: boolean;
  availableStock: number;
}

export interface ProductWithBatches extends Product {
  batches: Batch[];
}

export interface CartItem {
  productId: number;
  quantity: number;
}

export interface OrderItem {
  id: number;
  orderId: number;
  productId: number;
  batchId: number;
  quantity: number;
  unitPrice: number;
  totalPrice: number;
}

export interface Order {
  id: number;
  customerId: number;
  status: string;
  totalAmount: number;
  paymentStatus: string;
  createdAt: string;
  updatedAt: string;
  deliveryAddress: string;
  items: OrderItem[];
}

export interface Address {
  id: number;
  customerId: number;
  label: string;
  street: string;
  city: string;
  state: string;
  postalCode: string;
  phone: string;
  isDefault: boolean;
}

export interface Prescription {
  id: number;
  customerId: number;
  orderId: number | null;
  filePath: string;
  status: 'pending' | 'approved' | 'rejected' | 'clarification_required';
  reviewerId: string | null;
  decisionReason: string | null;
  createdAt: string;
  reviewedAt: string | null;
}

export interface ApiResponse<T> {
  data?: T;
  error?: string;
  message?: string;
}

const API_BASE = 'http://localhost:8080/api';

async function request<T>(
  path: string,
  options: RequestInit = {}
): Promise<ApiResponse<T>> {
  const token = localStorage.getItem('token');
  const headers: Record<string, string> = {
    'Content-Type': 'application/json',
    ...(options.headers as Record<string, string>),
  };
  if (token) {
    headers['Authorization'] = `Bearer ${token}`;
  }

  try {
    const res = await fetch(`${API_BASE}${path}`, {
      ...options,
      headers,
    });
    const body = await res.json().catch(() => ({}));
    if (!res.ok) {
      return { error: body.error || `Request failed with status ${res.status}` };
    }
    return { data: body as T, message: body.message };
  } catch (err) {
    return { error: err instanceof Error ? err.message : 'Network error' };
  }
}

export const api = {
  get: <T>(path: string) => request<T>(path, { method: 'GET' }),
  post: <T>(path: string, body?: unknown) =>
    request<T>(path, { method: 'POST', body: body ? JSON.stringify(body) : undefined }),
  put: <T>(path: string, body?: unknown) =>
    request<T>(path, { method: 'PUT', body: body ? JSON.stringify(body) : undefined }),
  delete: <T>(path: string) => request<T>(path, { method: 'DELETE' }),
};

export const authApi = {
  register: (data: { email: string; password: string; fullName: string; phone: string; role?: UserRole }) =>
    api.post<User>('/auth/register', data),
  login: (data: { email: string; password: string }) =>
    api.post<{ token: string; user: User }>('/auth/login', data),
  me: () => api.get<User>('/auth/me'),
};

export const productApi = {
  list: async (params?: { category?: string; search?: string; prescriptionRequired?: boolean }): Promise<ApiResponse<Product[]>> => {
    const qs = new URLSearchParams();
    if (params?.category) qs.set('category', params.category);
    if (params?.search) qs.set('search', params.search);
    if (params?.prescriptionRequired !== undefined) qs.set('prescriptionRequired', String(params.prescriptionRequired));
    const res = await api.get<{ products?: Product[] } | Product[]>(`/products?${qs.toString()}`);
    if (res.data) {
      const items = Array.isArray(res.data) ? res.data : (res.data.products ?? []);
      return { data: items, message: res.message, error: res.error };
    }
    return { error: res.error, message: res.message };
  },
  get: (id: number) => api.get<ProductWithBatches>(`/products/${id}`),
  search: (q: string) => api.get<Product[]>(`/products/search?q=${encodeURIComponent(q)}`),
};

export const cartApi = {
  get: () => api.get<{ items: CartItem[] }>('/cart'),
  add: (item: CartItem) => api.post('/cart/items', item),
  update: (productId: number, quantity: number) =>
    api.put(`/cart/items/${productId}`, { quantity }),
  remove: (productId: number) => api.delete(`/cart/items/${productId}`),
  clear: () => api.delete('/cart'),
};

export const orderApi = {
  list: () => api.get<Order[]>(`/orders`),
  get: (id: number) => api.get<Order>(`/orders/${id}`),
  create: (data: { customerId: number; items: { productId: number; quantity: number }[]; deliveryAddress: string }) =>
    api.post<{ id: number; message: string }>('/orders', data),
  cancel: (id: number) => api.post(`/orders/${id}/cancel`),
};

export const prescriptionApi = {
  list: () => api.get<Prescription[]>(`/prescriptions`),
  upload: (data: { orderId?: number; file: File }) => {
    const form = new FormData();
    if (data.orderId) form.append('orderId', String(data.orderId));
    form.append('file', data.file);
    return request<Prescription>('/prescriptions/upload', {
      method: 'POST',
      headers: {} as Record<string, string>,
      body: form,
    });
  },
};
