exports.up = function(knex, Promise) {
  return knex.raw(`
  CREATE TABLE "user_session" (
    id uuid PRIMARY KEY DEFAULT uuid_generate_v4(),
    user_agent text NULL,
    ip_address text NULL,
    user_id character varying(22) NOT NULL,
    refresh_token text NOT NULL UNIQUE,
    expires_at timestamp with time zone NOT NULL DEFAULT (now() + INTERVAL '15 days'),
    created_at timestamp with time zone NOT NULL DEFAULT now(),
    updated_at timestamp with time zone NOT NULL DEFAULT now(),
    FOREIGN KEY (user_id) REFERENCES "user" (id)
  );
 `);
};

exports.down = function(knex, Promise) {
  return knex.raw(`
    DROP TABLE IF EXISTS "user_session";
 `);
};
