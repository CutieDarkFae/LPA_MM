# replit.md

## Overview

This is a full-stack TypeScript web application using a React frontend with Express backend. The project follows a monorepo structure with clear separation between client, server, and shared code. It's designed as a starter template with authentication scaffolding and a comprehensive UI component library built on shadcn/ui.

## User Preferences

Preferred communication style: Simple, everyday language.

## System Architecture

### Frontend Architecture
- **Framework**: React 18 with TypeScript
- **Routing**: Wouter (lightweight React router)
- **State Management**: TanStack React Query for server state
- **UI Components**: shadcn/ui component library built on Radix UI primitives
- **Styling**: Tailwind CSS with CSS variables for theming (supports light/dark mode)
- **Build Tool**: Vite with HMR support

### Backend Architecture
- **Framework**: Express.js with TypeScript
- **Runtime**: Node.js with tsx for development
- **API Pattern**: RESTful API with `/api` prefix convention
- **Static Serving**: Production serves built client assets from `dist/public`

### Data Layer
- **ORM**: Drizzle ORM with PostgreSQL dialect
- **Schema Location**: `shared/schema.ts` - shared between client and server
- **Validation**: Zod schemas generated from Drizzle schemas via drizzle-zod
- **Storage Pattern**: Repository pattern with `IStorage` interface (currently in-memory, designed for database swap)

### Project Structure
```
├── client/           # React frontend
│   └── src/
│       ├── components/ui/  # shadcn/ui components
│       ├── hooks/          # Custom React hooks
│       ├── lib/            # Utilities and query client
│       └── pages/          # Route components
├── server/           # Express backend
│   ├── index.ts      # Server entry point
│   ├── routes.ts     # API route registration
│   ├── storage.ts    # Data access layer
│   └── vite.ts       # Vite dev middleware
├── shared/           # Shared code (schemas, types)
│   └── schema.ts     # Database schema and Zod validators
└── migrations/       # Drizzle migration files
```

### Build System
- **Development**: `npm run dev` - runs tsx with Vite middleware for HMR
- **Production Build**: `npm run build` - bundles client with Vite, server with esbuild
- **Database Sync**: `npm run db:push` - pushes schema to database via Drizzle Kit

### Path Aliases
- `@/*` → `./client/src/*`
- `@shared/*` → `./shared/*`
- `@assets` → `./attached_assets`

## External Dependencies

### Database
- **PostgreSQL**: Required via `DATABASE_URL` environment variable
- **Session Store**: connect-pg-simple for Express sessions (available but not currently wired)

### UI/Component Libraries
- **Radix UI**: Full suite of accessible primitives (dialog, dropdown, tabs, etc.)
- **Embla Carousel**: Carousel functionality
- **Recharts**: Charting library
- **Lucide React**: Icon library
- **Vaul**: Drawer component
- **cmdk**: Command palette component

### Form Handling
- **React Hook Form**: Form state management
- **@hookform/resolvers**: Zod integration for validation

### Utilities
- **date-fns**: Date manipulation
- **clsx + tailwind-merge**: Class name utilities
- **nanoid**: ID generation
- **class-variance-authority**: Component variant management

### Development Tools
- **Replit Plugins**: Runtime error overlay, cartographer, dev banner (development only)