CREATE TABLE `user` (
  `id` varchar(22),
  `username` text not null,
  `name` varchar(255),
  `email` varchar(255),
  `avatar` varchar(255),
  `created_at` datetime not null default CURRENT_TIMESTAMP,
  `updated_at` datetime not null default CURRENT_TIMESTAMP,
  primary key (`id`)
);
CREATE TABLE `user_identity_type` (
  `user_identity_type` varchar(255),
  primary key (`user_identity_type`)
);
CREATE TABLE `user_identity` (
  `user_id` varchar(22) not null,
  `identity_type` varchar(255) not null,
  `identity_id` varchar(255) not null,
  `data` json not null,
  foreign key(`user_id`) references `user`(`id`),
  foreign key(`identity_type`) references `user_identity_type`(`user_identity_type`),
  primary key (`user_id`, `identity_type`)
);
CREATE TABLE `note` (
  `id` varchar(22) not null,
  `user_id` varchar(22) not null,
  `title` text,
  `data` json,
  `created_at` datetime not null default CURRENT_TIMESTAMP,
  `updated_at` datetime not null default CURRENT_TIMESTAMP,
  `fork_from` varchar(22),
  `is_published` boolean not null default '0',
  `slug` varchar(255),
  foreign key(`user_id`) references `user`(`id`),
  primary key (`id`)
);
CREATE TABLE `note_revision` (
  `note_id` varchar(22) not null,
  `created_at` datetime not null default CURRENT_TIMESTAMP,
  `title` text,
  `data` json,
  foreign key(`note_id`) references `note`(`id`),
  primary key (`note_id`, `created_at`)
);
CREATE TABLE `note_edit_token` (
  `token` varchar(22) not null,
  `note_id` varchar(22) not null,
  foreign key(`note_id`) references `note`(`id`),
  primary key (`token`, `note_id`)
);